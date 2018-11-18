
#include "journal.h"
#include "util.h"
#include "table.h"
#include "entry.h"
#include <limits.h>

/*
 * Actualization of the journal structure
 */
struct ledger_journal {
  unsigned char *name;
  unsigned char *description;
  int item_id;
  struct ledger_table *table;
  /*
   * brief: entry count
   */
  int entry_count;
  /*
   * brief: array of entries
   */
  struct ledger_entry** entries;
  /*
   * next id to use
   */
  int sequence_id;
};

static int ledger_journal_schema[] =
  { LEDGER_TABLE_ID /* transaction ID */,
    LEDGER_TABLE_ID /* ledger ID */,
    LEDGER_TABLE_ID /* account ID */,
    LEDGER_TABLE_BIGNUM /* amount (+ debit, - credit) */,
    LEDGER_TABLE_USTR /* check number */,
    LEDGER_TABLE_USTR /* date */
  };

/*
 * Initialize a journal.
 * - a journal to initialize
 * @return one on success, zero on failure
 */
static int ledger_journal_init(struct ledger_journal* a);

/*
 * Clear out a journal.
 * - a journal to clear
 */
static void ledger_journal_clear(struct ledger_journal* a);


/* BEGIN static implementation */

int ledger_journal_init(struct ledger_journal* a){
  /* prepare the table */{
    int ok = 0;
    int const schema_size = sizeof(ledger_journal_schema)/
        sizeof(ledger_journal_schema[0]);
    struct ledger_table *new_table = ledger_table_new();
    if (new_table == NULL) return 0;
    else do {
      if (!ledger_table_set_column_types
          (new_table, schema_size, ledger_journal_schema))
        break;
      ok = 1;
    } while (0);
    if (!ok){
      ledger_table_free(new_table);
      return 0;
    } else {
      a->table = new_table;
    }
  }
  a->description = NULL;
  a->name = NULL;
  a->item_id = -1;
  a->sequence_id = 0;
  a->entries = NULL;
  a->entry_count = 0;
  return 1;
}

void ledger_journal_clear(struct ledger_journal* a){
  ledger_journal_set_entry_count(a,0);
  ledger_table_free(a->table);
  a->table = NULL;
  ledger_util_free(a->description);
  a->description = NULL;
  ledger_util_free(a->name);
  a->name = NULL;
  a->item_id = -1;
  a->sequence_id = 0;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_journal* ledger_journal_new(void){
  struct ledger_journal* a = (struct ledger_journal* )ledger_util_malloc
    (sizeof(struct ledger_journal));
  if (a != NULL){
    if (!ledger_journal_init(a)){
      ledger_util_free(a);
      a = NULL;
    }
  }
  return a;
}

void ledger_journal_free(struct ledger_journal* a){
  if (a != NULL){
    ledger_journal_clear(a);
    ledger_util_free(a);
  }
}

unsigned char const* ledger_journal_get_description
  (struct ledger_journal const* a)
{
  return a->description;
}

int ledger_journal_set_description
  (struct ledger_journal* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->description);
    a->description = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_journal_get_name
  (struct ledger_journal const* a)
{
  return a->name;
}

int ledger_journal_set_name
  (struct ledger_journal* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->name);
    a->name = new_desc;
    return 1;
  } else return 0;
}

int ledger_journal_get_id(struct ledger_journal const* a){
  return a->item_id;
}

void ledger_journal_set_id(struct ledger_journal* a, int item_id){
  if (item_id < 0){
    a->item_id = -1;
  } else {
    a->item_id = item_id;
  }
  return;
}

int ledger_journal_is_equal
  (struct ledger_journal const* a, struct ledger_journal const* b)
{
  /* trivial journals */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (a->item_id != b->item_id)
      return 0;
    if (ledger_util_ustrcmp(a->name, b->name) != 0)
      return 0;
    if (ledger_util_ustrcmp(a->description, b->description) != 0)
      return 0;
  }
  /* compare tables */{
    if (!ledger_table_is_equal(a->table, b->table))
      return 0;
  }
  /* compare entries */{
    int i;
    if (a->entry_count != b->entry_count) return 0;
    else for (i = 0; i < a->entry_count; ++i){
      if (!ledger_entry_is_equal(a->entries[i], b->entries[i]))
        break;
    }
    if (i < a->entry_count) return 0;
  }
  return 1;
}

struct ledger_table* ledger_journal_get_table(struct ledger_journal* a){
  return a->table;
}

struct ledger_table const* ledger_journal_get_table_c
  (struct ledger_journal const* a)
{
  return a->table;
}


int ledger_journal_get_sequence(struct ledger_journal const* a){
  return a->sequence_id;
}

int ledger_journal_set_sequence(struct ledger_journal* a, int item_id){
  if (item_id < 0) return 0;
  a->sequence_id = item_id;
  return 1;
}

int ledger_journal_alloc_id(struct ledger_journal* a){
  if (a->sequence_id < INT_MAX){
    int out;
    out = a->sequence_id;
    a->sequence_id += 1;
    return out;
  } else return -1;
}

int ledger_journal_get_entry_count(struct ledger_journal const* a){
  return a->entry_count;
}
struct ledger_entry* ledger_journal_get_entry
  (struct ledger_journal* a, int i)
{
  if (i < 0 || i >= a->entry_count){
    return NULL;
  } else {
    return a->entries[i];
  }
}
struct ledger_entry const* ledger_journal_get_entry_c
  (struct ledger_journal const* a, int i)
{
  if (i < 0 || i >= a->entry_count){
    return NULL;
  } else {
    return a->entries[i];
  }
}
int ledger_journal_set_entry_count(struct ledger_journal* a, int n){
  if (n >= INT_MAX/sizeof(struct ledger_entry*)){
    return 0;
  } else if (n < 0){
    return 0;
  } else if (n == 0){
    int i;
    for (i = 0; i < a->entry_count; ++i){
      ledger_entry_free(a->entries[i]);
    }
    ledger_util_free(a->entries);
    a->entries = NULL;
    a->entry_count = 0;
    return 1;
  } else if (n < a->entry_count){
    int i;
    /* allocate smaller array */
    struct ledger_entry** new_array = (struct ledger_entry** )
      ledger_util_malloc(n*sizeof(struct ledger_entry*));
    if (new_array == NULL) return 0;
    /* move old entries to new array */
    for (i = 0; i < n; ++i){
      new_array[i] = a->entries[i];
    }
    /* free rest of the entries */
    for (; i < a->entry_count; ++i){
      ledger_entry_free(a->entries[i]);
    }
    ledger_util_free(a->entries);
    a->entries = new_array;
    a->entry_count = n;
    return 1;
  } else if (n >= a->entry_count){
    int save_id;
    int i;
    /* allocate larger array */
    struct ledger_entry** new_array = (struct ledger_entry** )
      ledger_util_malloc(n*sizeof(struct ledger_entry*));
    if (new_array == NULL) return 0;
    /* save the sequence number in case of rollback */
    save_id = a->sequence_id;
    /* make new entries */
    for (i = a->entry_count; i < n; ++i){
      int next_id = ledger_journal_alloc_id(a);
      if (next_id == -1) break;
      new_array[i] = ledger_entry_new();
      if (new_array[i] == NULL) break;
      ledger_entry_set_id(new_array[i], next_id);
    }
    /* rollback and quit */if (i < n){
      int j;
      /* rollback */
      for (j = a->entry_count; j < i; ++j){
        ledger_entry_free(new_array[i]);
      }
      a->sequence_id = save_id;
      /* quit */
      return 0;
    }
    /* transfer old entries */
    for (i = 0; i < a->entry_count; ++i){
      new_array[i] = a->entries[i];
    }
    /* continue */
    ledger_util_free(a->entries);
    a->entries = new_array;
    a->entry_count = n;
    return 1;
  } else return 1 /*since n == a->entry_count */;
}


/* END   implementation */
