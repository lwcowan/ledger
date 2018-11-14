
#include "table.h"
#include "util.h"

/*
 * Table item
 */
union ledger_table_item {
  /* identifier */
  int item_id;
  /* cash amount */
  struct ledger_bignum* bignum;
  /* UTF-8 string */
  unsigned char* string;
};

/*
 * Table row
 */
struct ledger_table_row {
  /* link to previous row */
  struct ledger_table_row* prev;
  /* link to next row */
  struct ledger_table_row* next;
  /* data content */
  union ledger_table_item* data;
};

/*
 * Actualization of the table structure
 */
struct ledger_table {
  /* column line */
  int columns;
  /* row count cache */
  int rows;
  /* column types */
  unsigned char* column_types;
  /* double-linked list of table rows */
  struct ledger_table_row root;
};

struct ledger_table_iterator {
  struct ledger_table* source;
  struct ledger_table_row* row_ptr;
};

/*
 * Initialize a table.
 * - t table to initialize
 * @return one on success, zero on failure
 */
static int ledger_table_init(struct ledger_table* t);

/*
 * Clear out a table.
 * - t table to clear
 */
static void ledger_table_clear(struct ledger_table* t);


/* BEGIN static implementation */

int ledger_table_init(struct ledger_table* t){
  t->columns = 0;
  t->rows = 0;
  t->column_types = NULL;
  t->root.prev = &t->root;
  t->root.next = &t->root;
  t->root.data = NULL;
  return 1;
}

void ledger_table_clear(struct ledger_table* t){
  ledger_util_free(t->column_types);
  t->column_types = NULL;
  t->columns = 0;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_table* ledger_table_new(void){
  struct ledger_table* t = (struct ledger_table* )ledger_util_malloc
    (sizeof(struct ledger_table));
  if (t != NULL){
    if (!ledger_table_init(t)){
      ledger_util_free(t);
      t = NULL;
    }
  }
  return t;
}

void ledger_table_free(struct ledger_table* t){
  if (t != NULL){
    ledger_table_clear(t);
    ledger_util_free(t);
  }
}


int ledger_table_is_equal
  (struct ledger_table const* a, struct ledger_table const* b)
{
  /* trivial tables */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (a->rows != b->rows)
      return 0;
    if (a->columns != b->columns)
      return 0;
    if (memcmp(a->column_types, b->column_types,
        sizeof(unsigned char)*a->columns) != 0)
      return 0;
  }
  return 1;
}

/* END   implementation */
