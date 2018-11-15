
#include "table.h"
#include "util.h"
#include "bignum.h"
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
  /* data content (C99 feature) */
  union ledger_table_item data[];
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

/*
 * Construct a row given a schema.
 * - n row length
 * - schema array of data types
 * @return the row on success
 */
static struct ledger_table_row* ledger_table_row_new
  (int n, int const* schema);

/*
 * Destroy a row given a schema.
 * - r row to free
 * - n row length
 * - schema array of data types
 */
static void ledger_table_row_free
  (struct ledger_table_row* r, int n, int const* schema);



/* BEGIN static implementation */

int ledger_table_init(struct ledger_table* t){
  t->columns = 0;
  t->rows = 0;
  t->column_types = NULL;
  t->root.prev = &t->root;
  t->root.next = &t->root;
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

struct ledger_table_row* ledger_table_row_new(int n, int const* schema){
  struct ledger_table_row* new_row = (struct ledger_table_row*)
      ledger_util_malloc
        (sizeof(struct ledger_table_row)+n*sizeof(union ledger_table_item));
  if (new_row != NULL){
    int ok = 0;
    do {
      /* initialize each element in the row */
      int i;
      int entry_ok = 0;
      for (i = 0; i < n; ++i){
        switch (schema[i]){
        case LEDGER_TABLE_ID:
          new_row->data[i].item_id = 0;
          entry_ok = 1;
          break;
        case LEDGER_TABLE_BIGNUM:
          new_row->data[i].bignum = NULL;
          entry_ok = 1;
          break;
        case LEDGER_TABLE_USTR:
          new_row->data[i].string = NULL;
          entry_ok = 1;
          break;
        }
        if (!entry_ok) break;
      }
      if (!entry_ok) break;
      ok = 1;
    } while (0);
    if (!ok){
      ledger_util_free(new_row);
      new_row = NULL;
    }
  }
  return new_row;
}

void ledger_table_row_free
  (struct ledger_table_row* r, int n, int const* schema)
{
  /* free the row entries */{
    int i;
    for (i = 0; i < n; ++i){
      switch (schema[i]){
      case LEDGER_TABLE_ID:
        r->data[i].item_id = 0;
        break;
      case LEDGER_TABLE_BIGNUM:
        ledger_bignum_free(r->data[i].bignum);
        break;
      case LEDGER_TABLE_USTR:
        ledger_util_free(r->data[i].string);
        break;
      }
    }
  }
  /* free the row */{
    ledger_util_free(r);
  }
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

int ledger_table_count_rows(struct ledger_table const* t){
  return t->rows;
}

int ledger_table_add_row(struct ledger_table_mark* mark){
  if (mark->mutable_flag){
    int result = 0;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_row *new_row = NULL;
    do {
      /* allocate the row */
      new_row = ledger_table_row_new(table->columns, table->column_types);
      if (new_row == NULL) break;
      /* attach the row */
      new_row->prev = old_row->prev;
      new_row->next = old_row;
      old_row->prev->next = new_row;
      old_row->prev = new_row;
      /* set mark to new row */
      mark->row = new_row;
      /* cache the new row count */
      table->rows += 1;
      result = 1;
    } while (0);
    if (!result){
      ledger_table_row_free(new_row, table->columns, table->column_types);
    }
    return result;
  } else return 0;
}

int ledger_table_drop_row(struct ledger_table_mark* mark){
  if (mark->mutable_flag){
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    if (old_row == &table->root){
      /* don't allow it */;
      result = 0;
    } else /* remove the row */{
      /* detach the row */
      old_row->prev->next = old_row->next;
      old_row->next->prev = old_row->prev;
      /* move the mark */
      mark->row = old_row->prev;
      /* free the row */
      ledger_table_row_free(old_row, table->columns, table->column_types);
      /* cache the new row count */
      table->rows -= 1;
      result = 1;
    }
    return result;
  } else return 0;
}


/* END   implementation */
