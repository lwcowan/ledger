
#include "account.h"
#include "util.h"
#include "table.h"

/*
 * Actualization of the account structure
 */
struct ledger_account {
  unsigned char *name;
  unsigned char *description;
  int item_id;
  struct ledger_table *table;
};

static int ledger_account_schema[] =
  { LEDGER_TABLE_ID /* journal ID */,
    LEDGER_TABLE_ID /* transaction ID */,
    LEDGER_TABLE_BIGNUM /* amount (+ debit, - credit) */,
    LEDGER_TABLE_USTR /* check number */,
    LEDGER_TABLE_USTR /* date */
  };

/*
 * Initialize an account.
 * - a account to initialize
 * @return one on success, zero on failure
 */
static int ledger_account_init(struct ledger_account* a);

/*
 * Clear out an account.
 * - a account to clear
 */
static void ledger_account_clear(struct ledger_account* a);


/* BEGIN static implementation */

int ledger_account_init(struct ledger_account* a){
  /* prepare the table */{
    int ok = 0;
    int const schema_size = sizeof(ledger_account_schema)/
        sizeof(ledger_account_schema[0]);
    struct ledger_table *new_table = ledger_table_new();
    if (new_table == NULL) return 0;
    else do {
      if (!ledger_table_set_column_types
          (new_table, schema_size, ledger_account_schema))
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
  return 1;
}

void ledger_account_clear(struct ledger_account* a){
  ledger_table_free(a->table);
  a->table = NULL;
  ledger_util_free(a->description);
  a->description = NULL;
  ledger_util_free(a->name);
  a->description = NULL;
  a->item_id = -1;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_account* ledger_account_new(void){
  struct ledger_account* a = (struct ledger_account* )ledger_util_malloc
    (sizeof(struct ledger_account));
  if (a != NULL){
    if (!ledger_account_init(a)){
      ledger_util_free(a);
      a = NULL;
    }
  }
  return a;
}

void ledger_account_free(struct ledger_account* a){
  if (a != NULL){
    ledger_account_clear(a);
    ledger_util_free(a);
  }
}

unsigned char const* ledger_account_get_description
  (struct ledger_account const* a)
{
  return a->description;
}

int ledger_account_set_description
  (struct ledger_account* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->description);
    a->description = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_account_get_name
  (struct ledger_account const* a)
{
  return a->name;
}

int ledger_account_set_name
  (struct ledger_account* a, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(a->name);
    a->name = new_desc;
    return 1;
  } else return 0;
}

int ledger_account_get_id(struct ledger_account const* a){
  return a->item_id;
}

void ledger_account_set_id(struct ledger_account* a, int item_id){
  if (item_id < 0){
    a->item_id = -1;
  } else {
    a->item_id = item_id;
  }
  return;
}

int ledger_account_is_equal
  (struct ledger_account const* a, struct ledger_account const* b)
{
  /* trivial accounts */
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
  return 1;
}

struct ledger_table* ledger_account_get_table(struct ledger_account* a){
  return a->table;
}

struct ledger_table const* ledger_account_get_table_c
  (struct ledger_account const* a)
{
  return a->table;
}

/* END   implementation */
