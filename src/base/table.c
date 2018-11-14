
#include "table.h"
#include "util.h"
#include <string.h>
#include <limits.h>


/*
 * sanity check on digit count
 */
#ifndef LEDGER_TABLE_SCHEMA_MAX
#  define LEDGER_TABLE_SCHEMA_MAX 100
#endif /*LEDGER_TABLE_SCHEMA_MAX*/
#if LEDGER_TABLE_SCHEMA_MAX > (INT_MAX/(8*16))
#  error "LEDGER_TABLE_SCHEMA_MAX too large"
#endif /*LEDGER_TABLE_SCHEMA_MAX*/

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
  int* column_types;
  /* double-linked list of table rows */
  struct ledger_table_row root;
};

/*
 * Table row iterator.
 */
struct ledger_table_mark {
  struct ledger_table const* source;
  struct ledger_table_row* row;
  unsigned char mutable_flag;
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

/*
 * Construct a new mark.
 * - t table to use
 * - row row pointer
 * @return the mark on success, NULL otherwise
 */
static struct ledger_table_mark* ledger_table_mark_new
  ( struct ledger_table const* t, struct ledger_table_row const* r,
    int mutable_flag);



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

struct ledger_table_mark* ledger_table_mark_new
  ( struct ledger_table const* t, struct ledger_table_row const* r,
    int mutable_flag)
{
  struct ledger_table_mark* ptr = (struct ledger_table_mark*)
    ledger_util_malloc(sizeof(struct ledger_table_mark));
  if (ptr != NULL){
    ptr->source = t;
    ptr->row = (struct ledger_table_row*)r;
    ptr->mutable_flag = mutable_flag;
  }
  return ptr;
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

struct ledger_table_mark* ledger_table_begin(struct ledger_table* t){
  return ledger_table_mark_new(t, t->root.next, 1);
}

struct ledger_table_mark* ledger_table_begin_c
  (struct ledger_table const* t)
{
  return ledger_table_mark_new(t, t->root.next, 0);
}

struct ledger_table_mark* ledger_table_end(struct ledger_table* t){
  return ledger_table_mark_new(t, &t->root, 1);
}

struct ledger_table_mark* ledger_table_end_c(struct ledger_table const* t){
  return ledger_table_mark_new(t, &t->root, 0);
}

int ledger_table_mark_is_equal
  (struct ledger_table_mark const* a, struct ledger_table_mark const* b)
{
  return (a->source == b->source &&  a->row == b->row);
}

void ledger_table_mark_free(struct ledger_table_mark* m){
  ledger_util_free(m);
}

int ledger_table_get_column_count(struct ledger_table const* t){
  return t->columns;
}

int ledger_table_get_column_type(struct ledger_table const* t, int i){
  if (i < 0 || i >= t->columns) return 0;
  else return t->column_types[i];
}

int ledger_table_set_column_types
  (struct ledger_table* t, int n, int const* types)
{
  int *new_schema;
  /* validate the types */{
    int i;
    if (n > LEDGER_TABLE_SCHEMA_MAX) return 0;
    for (i = 0; i < n; ++i){
      if (types[i] >= 1 && types[i] <= 3)
        continue;
      else
        break;
    }
    if (i < n) /* invalid schema so */return 0;
  }
  /* allocate the column schema */{
    new_schema = (int*)ledger_util_malloc(n*sizeof(int));
    if (new_schema == NULL) return 0;
    else {
      if (n > 0)
        memcpy(new_schema,types,sizeof(int)*n);
    }
  }
  /* TODO reset the rows */{
  }
  /* store the new schema */{
    ledger_util_free(t->column_types);
    t->column_types = new_schema;
    t->columns = n;
  }
  return 1;
}

/* END   implementation */
