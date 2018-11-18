
#include "transact.h"
#include "../base/util.h"
#include "../base/table.h"
#include <limits.h>

/*
 * Actualization of the transaction structure
 */
struct ledger_transaction {
  unsigned char *name;
  unsigned char *description;
  struct ledger_table *table;
  int journal_id;
};

static int ledger_transaction_schema[] =
  { LEDGER_TABLE_ID /* ledger ID */,
    LEDGER_TABLE_ID /* account ID */,
    LEDGER_TABLE_BIGNUM /* amount (+ debit, - credit) */,
    LEDGER_TABLE_USTR /* check number */,
    LEDGER_TABLE_USTR /* date */
  };

/*
 * Initialize a transaction.
 * - a transaction to initialize
 * @return one on success, zero on failure
 */
static int ledger_transaction_init(struct ledger_transaction* a);

/*
 * Clear out a transaction.
 * - a transaction to clear
 */
static void ledger_transaction_clear(struct ledger_transaction* a);


/* BEGIN static implementation */

int ledger_transaction_init(struct ledger_transaction* a){
  /* prepare the table */{
    int ok = 0;
    int const schema_size = sizeof(ledger_transaction_schema)/
        sizeof(ledger_transaction_schema[0]);
    struct ledger_table *new_table = ledger_table_new();
    if (new_table == NULL) return 0;
    else do {
      if (!ledger_table_set_column_types
          (new_table, schema_size, ledger_transaction_schema))
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
  a->journal_id = -1;
  return 1;
}

void ledger_transaction_clear(struct ledger_transaction* a){
  ledger_table_free(a->table);
  a->table = NULL;
  ledger_util_free(a->description);
  a->description = NULL;
  ledger_util_free(a->name);
  a->name = NULL;
  a->journal_id = -1;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_transaction* ledger_transaction_new(void){
  struct ledger_transaction* a = (struct ledger_transaction* )ledger_util_malloc
    (sizeof(struct ledger_transaction));
  if (a != NULL){
    if (!ledger_transaction_init(a)){
      ledger_util_free(a);
      a = NULL;
    }
  }
  return a;
}

void ledger_transaction_free(struct ledger_transaction* a){
  if (a != NULL){
    ledger_transaction_clear(a);
    ledger_util_free(a);
  }
}

unsigned char const* ledger_transaction_get_description
  (struct ledger_transaction const* a)
{
  return a->description;
}

int ledger_transaction_set_description
  (struct ledger_transaction* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->description);
    a->description = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_transaction_get_name
  (struct ledger_transaction const* a)
{
  return a->name;
}

int ledger_transaction_set_name
  (struct ledger_transaction* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->name);
    a->name = new_desc;
    return 1;
  } else return 0;
}

int ledger_transaction_get_journal(struct ledger_transaction const* a){
  return a->journal_id;
}

void ledger_transaction_set_journal(struct ledger_transaction* a, int journal_id){
  if (journal_id < 0){
    a->journal_id = -1;
  } else {
    a->journal_id = journal_id;
  }
  return;
}

int ledger_transaction_is_equal
  (struct ledger_transaction const* a, struct ledger_transaction const* b)
{
  /* trivial transactions */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (a->journal_id != b->journal_id)
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
  return 1;
}

struct ledger_table* ledger_transaction_get_table
  (struct ledger_transaction* a)
{
  return a->table;
}

struct ledger_table const* ledger_transaction_get_table_c
  (struct ledger_transaction const* a)
{
  return a->table;
}


/* END   implementation */
