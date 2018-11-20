
#include "entry.h"
#include "util.h"

/*
 * Actualization of the entry structure
 */
struct ledger_entry {
  unsigned char *name;
  unsigned char *description;
  unsigned char *date;
  int item_id;
};

/*
 * Initialize an entry.
 * - a entry to initialize
 * @return one on success, zero on failure
 */
static int ledger_entry_init(struct ledger_entry* a);

/*
 * Clear out an entry.
 * - a entry to clear
 */
static void ledger_entry_clear(struct ledger_entry* a);


/* BEGIN static implementation */

int ledger_entry_init(struct ledger_entry* a){
  a->description = NULL;
  a->name = NULL;
  a->item_id = -1;
  a->date = NULL;
  return 1;
}

void ledger_entry_clear(struct ledger_entry* a){
  ledger_util_free(a->description);
  a->description = NULL;
  ledger_util_free(a->name);
  a->description = NULL;
  ledger_util_free(a->date);
  a->date = NULL;
  a->item_id = -1;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_entry* ledger_entry_new(void){
  struct ledger_entry* a = (struct ledger_entry* )ledger_util_malloc
    (sizeof(struct ledger_entry));
  if (a != NULL){
    if (!ledger_entry_init(a)){
      ledger_util_free(a);
      a = NULL;
    }
  }
  return a;
}

void ledger_entry_free(struct ledger_entry* a){
  if (a != NULL){
    ledger_entry_clear(a);
    ledger_util_free(a);
  }
}

unsigned char const* ledger_entry_get_description
  (struct ledger_entry const* a)
{
  return a->description;
}

int ledger_entry_set_description
  (struct ledger_entry* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->description);
    a->description = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_entry_get_name
  (struct ledger_entry const* a)
{
  return a->name;
}

int ledger_entry_set_name
  (struct ledger_entry* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->name);
    a->name = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_entry_get_date
  (struct ledger_entry const* a)
{
  return a->date;
}

int ledger_entry_set_date
  (struct ledger_entry* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->date);
    a->date = new_desc;
    return 1;
  } else return 0;
}

int ledger_entry_get_id(struct ledger_entry const* a){
  return a->item_id;
}

void ledger_entry_set_id(struct ledger_entry* a, int item_id){
  if (item_id < 0){
    a->item_id = -1;
  } else {
    a->item_id = item_id;
  }
  return;
}

int ledger_entry_is_equal
  (struct ledger_entry const* a, struct ledger_entry const* b)
{
  /* trivial entrys */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (a->item_id != b->item_id)
      return 0;
    if (ledger_util_ustrcmp(a->name, b->name) != 0)
      return 0;
    if (ledger_util_ustrcmp(a->description, b->description) != 0)
      return 0;
    if (ledger_util_ustrcmp(a->date, b->date) != 0)
      return 0;
  }
  return 1;
}

/* END   implementation */
