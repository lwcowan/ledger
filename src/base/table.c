
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
 * Row schema.
 */
struct ledger_table_schema {
  /* column line */
  int columns;
  /* column types */
  int types[];
};

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
  /* column schema */
  struct ledger_table_schema *schema;
  /* row count cache */
  int rows;
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
 * Callback for cleaning up a table.
 * - t pointer to a table
 */
static void ledger_table_free_cb(void* t);

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

/*
 * Drop all rows from a table.
 * - table table to clear out
 */
static void ledger_table_drop_all_rows(struct ledger_table* table);

/*
 * Construct a new table schema.
 * - columns number of columns to use
 * @return the schema on success, NULL otherwise
 */
static struct ledger_table_schema* ledger_table_schema_new
  (int columns, int const* types);

/*
 * Acquire a table schema.
 * - sch the schema to acquire
 * @return the schema on success, NULL otherwise
 */
static struct ledger_table_schema* ledger_table_schema_acquire
  (struct ledger_table_schema* sch);

/*
 * Clear out a schema.
 * - sch the schema to clear
 */
static void ledger_table_schema_clear(struct ledger_table_schema* sch);

/*
 * Release a schema.
 * - sch the schema to release
 */
static void ledger_table_schema_free(struct ledger_table_schema* sch);

/*
 * Compare two schemata for equality.
 * - a one schema
 * - b another schema
 * @return nonzero if the schemata are equal, zero otherwise
 */
int ledger_table_schema_is_equal
  (struct ledger_table_schema const* a, struct ledger_table_schema const* b);


/* BEGIN static implementation */

void ledger_table_free_cb(void* t){
  ledger_table_clear((struct ledger_table*) t);
  return;
}

int ledger_table_init(struct ledger_table* t){
  /* NOTE pre-clear compatible */
  t->schema = NULL;
  t->rows = 0;
  t->root.prev = &t->root;
  t->root.next = &t->root;
  return 1;
}

void ledger_table_clear(struct ledger_table* t){
  ledger_table_drop_all_rows(t);
  ledger_table_schema_free(t->schema);
  t->schema = NULL;
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

void ledger_table_drop_all_rows(struct ledger_table* table){
  struct ledger_table_mark quick_mark;
  quick_mark.source = table;
  quick_mark.row = table->root.prev;
  quick_mark.mutable_flag = 1;
  while (quick_mark.row != &table->root){
    ledger_table_drop_row(&quick_mark);
  }
  return;
}

void ledger_table_schema_free_cb(void* ptr){
  ledger_table_schema_clear((struct ledger_table_schema*)ptr);
  return;
}

struct ledger_table_schema* ledger_table_schema_new
  (int columns, int const* types)
{
  /* NOTE pre-clear compatible */
  struct ledger_table_schema* new_schema = (struct ledger_table_schema*)
      ledger_util_ref_malloc
        ( sizeof(struct ledger_table_schema)+columns*sizeof(int),
          ledger_table_schema_free_cb);
  if (new_schema != NULL){
    int i;
    new_schema->columns = columns;
    for (i = 0; i < columns; ++i){
      new_schema->types[i] = types[i];
    }
    return new_schema;
  } else return NULL;
}

struct ledger_table_schema* ledger_table_schema_acquire
  (struct ledger_table_schema* sch)
{
  return (struct ledger_table_schema*)ledger_util_ref_acquire(sch);
}

void ledger_table_schema_free(struct ledger_table_schema* sch){
  if (sch != NULL){
    ledger_util_ref_free(sch);
  }
  return;
}

void ledger_table_schema_clear(struct ledger_table_schema* sch){
  int i;
  for (i = 0; i < sch->columns; ++i){
    sch->types[i] = -1;
  }
  return;
}

int ledger_table_schema_is_equal
  (struct ledger_table_schema const* a, struct ledger_table_schema const* b)
{
  /* trivial table schemata */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (a->columns != b->columns)
      return 0;
    if (memcmp(a->types, b->types, sizeof(int)*a->columns) != 0)
      return 0;
  }
  return 1;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_table* ledger_table_new(void){
  struct ledger_table* t = (struct ledger_table* )ledger_util_ref_malloc
    (sizeof(struct ledger_table), ledger_table_free_cb);
  if (t != NULL){
    if (!ledger_table_init(t)){
      ledger_util_ref_free(t);
      t = NULL;
    }
  }
  return t;
}

struct ledger_table* ledger_table_acquire(struct ledger_table* t){
  return (struct ledger_table*)ledger_util_ref_acquire(t);
}

void ledger_table_free(struct ledger_table* t){
  if (t != NULL){
    /*NOTE ledger_table_clear(t); called indirectly*/
    ledger_util_ref_free(t);
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
    if (!ledger_table_schema_is_equal(a->schema, b->schema))
      return 0;
  }
  /* compare row by row */if (a->schema != NULL){
    int const columns = a->schema->columns;
    int const* const schema = a->schema->types;
    struct ledger_table_mark a_mark = {a,a->root.next,0};
    struct ledger_table_mark b_mark = {b,b->root.next,0};
    for (; a_mark.row != &a->root && b_mark.row != &b->root;
         a_mark.row = a_mark.row->next, b_mark.row = b_mark.row->next)
    {
      /* perform field-wise comparison */
      int i;
      for (i = 0; i < columns; ++i){
        switch (schema[i]){
        case LEDGER_TABLE_ID:
          {
            if (a_mark.row->data[i].item_id != b_mark.row->data[i].item_id)
              return 0;
          }break;
        case LEDGER_TABLE_BIGNUM:
          {
            if (ledger_bignum_compare(
                a_mark.row->data[i].bignum, b_mark.row->data[i].bignum) != 0)
              return 0;
          }break;
        case LEDGER_TABLE_USTR:
          {
            if (ledger_util_ustrcmp(
                a_mark.row->data[i].string, b_mark.row->data[i].string) != 0)
              return 0;
          }break;
        }
      }
    }
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
  return t->schema != NULL ? t->schema->columns : 0;
}

int ledger_table_get_column_type(struct ledger_table const* t, int i){
  struct ledger_table_schema const* const s = t->schema;
  if (s == NULL || i < 0 || i >= s->columns) return 0;
  else return s->types[i];
}

int ledger_table_set_column_types
  (struct ledger_table* t, int n, int const* types)
{
  struct ledger_table_schema *new_schema;
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
    new_schema = ledger_table_schema_new(n, types);
    if (new_schema == NULL) return 0;
  }
  /* reset the rows */{
    ledger_table_drop_all_rows(t);
  }
  /* store the new schema */{
    ledger_table_schema_free(t->schema);
    t->schema = new_schema;
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
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    do {
      /* allocate the row */
      new_row = ledger_table_row_new(columns, types);
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
      ledger_table_row_free(new_row, schema->columns, schema->types);
    }
    return result;
  } else return 0;
}

int ledger_table_drop_row(struct ledger_table_mark* mark){
  if (mark->mutable_flag){
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
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
      ledger_table_row_free(old_row, columns, types);
      /* cache the new row count */
      table->rows -= 1;
      result = 1;
    }
    return result;
  } else return 0;
}

int ledger_table_fetch_string
  (struct ledger_table_mark const* mark, int i, unsigned char* buf, int len)
{
  /* const or mutable accepted */{
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row == &table->root
    ||  i < 0
    ||  i >= columns)
    {
      /* don't allow it */;
      result = -1;
    } else /* fetch the string */{
      switch (types[i]){
      case LEDGER_TABLE_ID:
        if (old_row->data[i].item_id < 0){
          result = 0;
          if (len > 0) buf[0] = 0;
        } else {
          result = (int)ledger_util_itoa(old_row->data[i].item_id,buf,len,0);
        }break;
      case LEDGER_TABLE_BIGNUM:
        if (old_row->data[i].bignum == NULL){
          result = 0;
          if (len > 0) buf[0] = 0;
        } else {
          result =
            ledger_bignum_get_text(old_row->data[i].bignum, buf, len,0);
        }break;
      case LEDGER_TABLE_USTR:
        if (old_row->data[i].string == NULL){
          result = 0;
          if (len > 0) buf[0] = 0;
        } else {
          result = ledger_util_ustrlen(old_row->data[i].string);
          if (len > 0){
            int truncated_result = result>=len?len-1:result;
            memcpy(buf, old_row->data[i].string,
                truncated_result*sizeof(unsigned char));
            buf[truncated_result] = 0;
          }
        }break;
      }
    }
    return result;
  }
}

int ledger_table_put_string
  (struct ledger_table_mark const* mark, int i, unsigned char const* value)
{
  if (mark->mutable_flag){
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row == &table->root
    ||  i < 0
    ||  i >= columns)
    {
      /* don't allow it */;
      result = -1;
    } else /* put the string */{
      switch (types[i]){
      case LEDGER_TABLE_ID:
        if (value == NULL || *value == 0){
          old_row->data[i].item_id = -1;
          result = 1;
        } else {
          old_row->data[i].item_id = ledger_util_atoi(value);
          result = 1;
        }break;
      case LEDGER_TABLE_BIGNUM:
        if (value == NULL || *value == 0){
          ledger_bignum_free(old_row->data[i].bignum);
          old_row->data[i].bignum = NULL;
          result = 1;
        } else {
          int is_new_ptr = 0;
          struct ledger_bignum* bignum = old_row->data[i].bignum;
          if (bignum == NULL){
            bignum = ledger_bignum_new();
            if (bignum == NULL) break;
            else is_new_ptr = 1;
          }
          /* put the big number */{
            result = ledger_bignum_set_text(bignum, value, NULL);
            if (result){
              old_row->data[i].bignum = bignum;
            } else if (is_new_ptr){
              ledger_bignum_free(bignum);
            }
          }
        }break;
      case LEDGER_TABLE_USTR:
        /* duplicate, free, replace */{
          unsigned char *new_string = ledger_util_ustrdup(value,&result);
          if (result){
            ledger_util_free(old_row->data[i].string);
            old_row->data[i].string = new_string;
          }
        }break;
      }
    }
    return result;
  } else return 0;
}

int ledger_table_fetch_bignum
  (struct ledger_table_mark const* mark, int i, struct ledger_bignum* n)
{
  /* const or mutable accepted */{
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row == &table->root
    ||  i < 0
    ||  i >= columns)
    {
      /* don't allow it */;
      result = -1;
    } else /* fetch the string */{
      switch (types[i]){
      case LEDGER_TABLE_ID:
        if (old_row->data[i].item_id < 0){
          result = ledger_bignum_set_long(n, -1);
        } else {
          result = ledger_bignum_set_long(n, old_row->data[i].item_id);
        }break;
      case LEDGER_TABLE_BIGNUM:
        if (old_row->data[i].bignum == NULL){
          result = ledger_bignum_set_long(n, 0);
        } else {
          result = ledger_bignum_assign(n, old_row->data[i].bignum);
        }break;
      case LEDGER_TABLE_USTR:
        if (old_row->data[i].string == NULL){
          result = ledger_bignum_set_long(n, 0);
        } else {
          result = ledger_bignum_set_text(n, old_row->data[i].string, NULL);
        }break;
      }
    }
    return result;
  }
}

int ledger_table_put_bignum
  ( struct ledger_table_mark const* mark, int i,
    struct ledger_bignum const* value)
{
  if (mark->mutable_flag){
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row == &table->root
    ||  i < 0
    ||  i >= columns)
    {
      /* don't allow it */;
      result = -1;
    } else /* put the string */{
      switch (types[i]){
      case LEDGER_TABLE_ID:
        if (value == NULL){
          old_row->data[i].item_id = 0;
          result = 1;
        } else {
          old_row->data[i].item_id = ledger_bignum_get_long(value);
          result = 1;
        }break;
      case LEDGER_TABLE_BIGNUM:
        if (value == NULL){
          ledger_bignum_free(old_row->data[i].bignum);
          old_row->data[i].bignum = NULL;
          result = 1;
        } else {
          int is_new_ptr = 0;
          struct ledger_bignum* bignum = old_row->data[i].bignum;
          if (bignum == NULL){
            bignum = ledger_bignum_new();
            if (bignum == NULL) break;
            else is_new_ptr = 1;
          }
          /* put the big number */{
            result = ledger_bignum_assign(bignum, value);
            if (result){
              old_row->data[i].bignum = bignum;
            } else if (is_new_ptr){
              ledger_bignum_free(bignum);
            }
          }
        }break;
      case LEDGER_TABLE_USTR:
        /* construct the string */{
          int len = ledger_bignum_get_text(value,NULL,0,0);
          if (len > 0){
            unsigned char *new_string = ledger_util_malloc(len+1);
            if (new_string != NULL){
              ledger_bignum_get_text(value,new_string,len+1,0);
              ledger_util_free(old_row->data[i].string);
              old_row->data[i].string = new_string;
              result = 1;
            } else result = 0;
          } else if (len == 0){
            ledger_util_free(old_row->data[i].string);
            old_row->data[i].string = NULL;
            result = 1;
          } else {
            result = 0;
          }
        }break;
      }
    }
    return result;
  } else return 0;
}

void ledger_table_mark_move(struct ledger_table_mark* m, int n){
  if (n > 0){
    int i;
    for (i = 0; i < n; ++i){
      m->row = m->row->next;
    }
  } else if (n < 0){
    int i;
    for (i = 0; i > n; --i){
      m->row = m->row->prev;
    }
  } else /* stay put and */return;
}


int ledger_table_fetch_id
  (struct ledger_table_mark const* mark, int i, int* n)
{
  /* const or mutable accepted */{
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row == &table->root
    ||  i < 0
    ||  i >= columns)
    {
      /* don't allow it */;
      result = -1;
    } else /* fetch the string */{
      switch (types[i]){
      case LEDGER_TABLE_ID:
        if (old_row->data[i].item_id < 0){
          *n = -1;
          result = 1;
        } else {
          *n = old_row->data[i].item_id;
          result = 1;
        }break;
      case LEDGER_TABLE_BIGNUM:
        if (old_row->data[i].bignum == NULL){
          *n = 0;
          result = 1;
        } else {
          *n = (int)ledger_bignum_get_long(old_row->data[i].bignum);
          result = 1;
        }break;
      case LEDGER_TABLE_USTR:
        if (old_row->data[i].string == NULL){
          *n = -1;
          result = 1;
        } else {
          *n = ledger_util_atoi(old_row->data[i].string);
          result = 1;
        }break;
      }
    }
    return result;
  }
}

int ledger_table_put_id
  ( struct ledger_table_mark const* mark, int i, int value)
{
  if (mark->mutable_flag){
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = table->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row == &table->root
    ||  i < 0
    ||  i >= columns)
    {
      /* don't allow it */;
      result = -1;
    } else /* put the string */{
      switch (types[i]){
      case LEDGER_TABLE_ID:
        if (value < 0){
          old_row->data[i].item_id = -1;
          result = 1;
        } else {
          old_row->data[i].item_id = value;
          result = 1;
        }break;
      case LEDGER_TABLE_BIGNUM:
        /* conversion is direct */{
          int is_new_ptr = 0;
          struct ledger_bignum* bignum = old_row->data[i].bignum;
          if (bignum == NULL){
            bignum = ledger_bignum_new();
            if (bignum == NULL) break;
            else is_new_ptr = 1;
          }
          /* put the big number */{
            result = ledger_bignum_set_long(bignum, value);
            if (result){
              old_row->data[i].bignum = bignum;
            } else if (is_new_ptr){
              ledger_bignum_free(bignum);
            }
          }
        }break;
      case LEDGER_TABLE_USTR:
        /* construct the string */{
          int len = ledger_util_itoa(value,NULL,0,0);
          if (len > 0){
            unsigned char *new_string = ledger_util_malloc(len+1);
            if (new_string != NULL){
              ledger_util_itoa(value,new_string,len+1,0);
              ledger_util_free(old_row->data[i].string);
              old_row->data[i].string = new_string;
              result = 1;
            } else result = 0;
          } else if (len == 0){
            ledger_util_free(old_row->data[i].string);
            old_row->data[i].string = NULL;
            result = 1;
          } else {
            result = 0;
          }
        }break;
      }
    }
    return result;
  } else return 0;
}


/* END   implementation */
