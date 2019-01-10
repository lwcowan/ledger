
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
  /* lock flag */
  int lock_tf;
  /* whether the schema is outdated (0-no, 1-drop only, 2-yes)*/
  int outdated_tf;
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
  /* row schema */
  struct ledger_table_schema* schema;
  /* root flag */
  int root_tf;
  /* data content (C99 feature) */
  union ledger_table_item data[];
};

/*
 * Actualization of the table structure
 */
struct ledger_table {
  /* lock value */
  int lock_tf;
  /* column schema */
  struct ledger_table_schema *schema;
  /* row count cache */
  int rows;
  /* double-linked list of table rows */
  struct ledger_table_row *root;
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
 * Lock a table for modification.
 * - t table to lock
 * @return one on success, zero on failure
 */
static void ledger_table_lock(struct ledger_table const* t);

/*
 * Lock two table schemata simultaneously for modification.
 * - t first table schema to lock
 * - t2 second table schema to lock
 * @return one on success, zero on failure
 */
static void ledger_table_schema_lock2
  (struct ledger_table_schema const* t, struct ledger_table_schema const* t2);

/*
 * Unlock a table for modification.
 * - t table to unlock
 */
static void ledger_table_unlock(struct ledger_table const* t);

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
 * Construct a new mark in place.
 * - t table to use
 * - row row pointer
 * - mutable_flag whether the mark should be allowed to modify the table
 * - ptr pointer to the mark structrue
 * @return one on success, zero otherwise
 */
static int ledger_table_mark_init
  ( struct ledger_table const* t, struct ledger_table_row const* r,
    int mutable_flag, struct ledger_table_mark* ptr);

/*
 * Clear a mark.
 * - m the mark to clear
 */
void ledger_table_mark_clear(struct ledger_table_mark* m);

/*
 * Construct a row given a schema.
 * - schema array of data types
 * @return the row on success
 */
static struct ledger_table_row* ledger_table_row_new
  (struct ledger_table_schema const* schema);

/*
 * Destroy a row.
 * - r row to free
 */
static void ledger_table_row_free
  (struct ledger_table_row* r);

/*
 * Destroy the contents of a row given a schema.
 * - r row to clear
 * - n row length
 * - schema array of data types
 */
static void ledger_table_row_clear
  (struct ledger_table_row* r, int n, int const* schema);

/*
 * Callback for row destruction.
 * - ptr pointer to row structure
 */
static void ledger_table_row_free_cb(void* ptr);

/*
 * Acquire a reference to a row.
 * - r target row
 * @return the row on success, NULL otherwise
 */
static struct ledger_table_row* ledger_table_row_acquire
  (struct ledger_table_row* r);


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

/*
 * Recursively lock a table schema for modification of rows.
 * - t table schema to lock
 */
static void ledger_table_schema_lock(struct ledger_table_schema const* t);

/*
 * Outdate a table schema.
 * - t table schema
 * @return whether the caller acquired closing rights
 */
static int ledger_table_schema_outdate(struct ledger_table_schema* t);

/*
 * Check whether a schema is outdated.
 * - t table schema to query
 * @return one if the schema is outdated, zero otherwise
 */
int ledger_table_schema_is_outdated(struct ledger_table_schema const* t);

/*
 * Unlock a table schema for modification.
 * - t table schema to unlock
 */
static void ledger_table_schema_unlock(struct ledger_table_schema const* t);

/*
 * Close a schema from further modification.
 * - t the schema to close
 */
void ledger_table_schema_close(struct ledger_table_schema* t);

/*
 * Subroutine for table row insertion.
 * - mark mark pointing to the neighboring row
 * @return one on success, zero otherwise
 */
static int ledger_table_add_row_sub(struct ledger_table_mark* mark);

/*
 * Subroutine for table row deletion.
 * - mark mark pointing to the row to delete
 * @return one on success, zero otherwise
 */
static int ledger_table_drop_row_sub(struct ledger_table_mark* mark);

/*
 * Subroutine for fetching strings from a field.
 * - mark mark pointing to the row to read
 * - i field index
 * - buf output buffer
 * - len length of buffer in bytes
 * @return one on success, zero otherwise
 */
static int ledger_table_fetch_string_sub
  (struct ledger_table_mark const* mark, int i, unsigned char* buf, int len);

/*
 * Subroutine for putting identifiers to a field.
 * - mark mark pointing to the row to write
 * - i field index
 * - value identifier to put
 * @return one on success, zero otherwise
 */
static int ledger_table_put_id_sub
  ( struct ledger_table_mark const* mark, int i, int value);

/*
 * Subroutine for fetching identifiers from a field.
 * - mark mark pointing to the row to read
 * - i field index
 * - n place to stored the fetched identifier
 * @return one on success, zero otherwise
 */
static int ledger_table_fetch_id_sub
  (struct ledger_table_mark const* mark, int i, int* n);

/*
 * Subroutine for putting big numbers to a field.
 * - mark mark pointing to the row to write
 * - i field index
 * - value number to put
 * @return one on success, zero otherwise
 */
static int ledger_table_put_bignum_sub
  ( struct ledger_table_mark const* mark, int i,
    struct ledger_bignum const* value);

/*
 * Subroutine for fetching big numbers from a field.
 * - mark mark pointing to the row to read
 * - i field index
 * - n place to stored the fetched number
 * @return one on success, zero otherwise
 */
static int ledger_table_fetch_bignum_sub
  (struct ledger_table_mark const* mark, int i, struct ledger_bignum* n);

/*
 * Subroutine for putting strings to a field.
 * - mark mark pointing to the row to write
 * - i field index
 * - value string to put
 * @return one on success, zero otherwise
 */
static int ledger_table_put_string_sub
  (struct ledger_table_mark const* mark, int i, unsigned char const* value);

/*
 * Exchange mark pointer.
 * - mark the mark to edit
 * - new_row the new row
 */
static void ledger_table_mark_exchange
  (struct ledger_table_mark *mark, struct ledger_table_row* new_row);

/*
 * Add one to the mark, ensuring that the mark is still valid.
 * - m the mark to advance
 * @return one on success, zero if the mark is caught
 */
static int ledger_table_mark_add_one_checked(struct ledger_table_mark* m);

/* BEGIN static implementation */

void ledger_table_lock(struct ledger_table const* t){
  ((struct ledger_table*)t)->lock_tf += 1;
  return;
}

void ledger_table_unlock(struct ledger_table const* t){
  ((struct ledger_table*)t)->lock_tf -= 1;
  return;
}

void ledger_table_free_cb(void* t){
  ledger_table_clear((struct ledger_table*) t);
  return;
}

int ledger_table_init(struct ledger_table* t){
  /* NOTE pre-clear compatible */
  t->schema = NULL;
  t->rows = 0;
  t->root = NULL;
  t->lock_tf = 0;
  /* allocate a root */{
    struct ledger_table_row* root;
    struct ledger_table_schema* schema = ledger_table_schema_new(0,NULL);
    if (schema == NULL) return 0;
    root = ledger_table_row_new(schema);
    if (root == NULL){
      ledger_table_schema_free(schema);
      return 0;
    }
    root->root_tf = 1;
    t->root = root;
    t->schema = schema;
  }
  /* POST_CONDITION table has a schema */
  return 1;
}

void ledger_table_clear(struct ledger_table* t){
  if (t->schema == NULL){
    /* rows never made */
  } else {
    /* if only one reference held on this table, then no lock necessary */
    if (ledger_table_schema_outdate(t->schema)){
      if (t->root != NULL){
        ledger_table_drop_all_rows(t);
      }
      ledger_table_schema_close(t->schema);
    }
    ledger_table_row_free(t->root);
    ledger_table_schema_free(t->schema);
    t->root = NULL;
    t->schema = NULL;
  }
  /*assert(t->lock_tf == 0);*/
  return;
}

struct ledger_table_mark* ledger_table_mark_new
  ( struct ledger_table const* t, struct ledger_table_row const* r,
    int mutable_flag)
{
  struct ledger_table_mark* ptr;
  ptr = (struct ledger_table_mark*)
    ledger_util_malloc(sizeof(struct ledger_table_mark));
  if (!ledger_table_mark_init(t,r,mutable_flag,ptr)){
    ledger_util_free(ptr);
    ptr = NULL;
  }
  return ptr;
}

int ledger_table_mark_init
  ( struct ledger_table const* t, struct ledger_table_row const* r,
    int mutable_flag, struct ledger_table_mark* ptr)
{
  struct ledger_table_row* row =
    ledger_table_row_acquire((struct ledger_table_row*)r);
  if (row == NULL){
    return 0;
  }
  /* put mark contents */{
    ptr->source = t;
    ptr->row = (struct ledger_table_row*)r;
    ptr->mutable_flag = mutable_flag;
  }
  return 1;
}

void ledger_table_mark_clear(struct ledger_table_mark* m){
  ledger_table_row_free(m->row);
  return;
}

int ledger_table_mark_add_one_checked(struct ledger_table_mark* m){
  int consistent_tf;
  struct ledger_table_row *const row = m->row;
  ledger_table_schema_lock(row->schema);
  if (row->next != row){
    ledger_table_mark_exchange(m, row->next);
    consistent_tf = 1;
  } else consistent_tf = 0;
  ledger_table_schema_unlock(row->schema);
  return consistent_tf;
}

void ledger_table_row_free_cb(void* ptr){
  struct ledger_table_row* r = (struct ledger_table_row*)ptr;
  /*assert (r->schema != NULL);*/{
    int n = r->schema->columns;
    int const* schema = r->schema->types;
    ledger_table_row_clear(r, n, schema);
  }
  return;
}

struct ledger_table_row* ledger_table_row_new
  (struct ledger_table_schema const* sch)
{
  struct ledger_table_schema* new_schema;
  struct ledger_table_row* new_row;
  if (sch == NULL) return NULL;
  else {
    new_schema =
      ledger_table_schema_acquire((struct ledger_table_schema*)sch);
    if (new_schema == NULL) return NULL;
  }
  /* allocate the row */{
    int const n = new_schema->columns;
    new_row = (struct ledger_table_row*)ledger_util_ref_malloc
        ( sizeof(struct ledger_table_row)+n*sizeof(union ledger_table_item),
          ledger_table_row_free_cb);
  }
  if (new_row != NULL){
    int const* const schema = new_schema->types;
    int const n = new_schema->columns;
    int ok = 0;
    do {
      int i;
      int entry_ok = 1;
      /* initialize row pointers */
      new_row->next = new_row;
      new_row->prev = new_row;
      new_row->schema = new_schema;
      new_row->root_tf = 0;
      /* initialize each element in the row */
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
      ledger_util_ref_free(new_row);
      new_row = NULL;
    }
  } else ledger_table_schema_free(new_schema);
  return new_row;
  /* NOTE POST_CONDITION: row has a schema */
}

void ledger_table_row_free(struct ledger_table_row* r){
  ledger_util_ref_free(r);
  return;
}

void ledger_table_row_clear
  (struct ledger_table_row* r, int n, int const* schema)
{
  /* NOTE PRE_CONDITION: row has a schema */
  /* detach the row */{
    ledger_table_schema_lock(r->schema);
    r->prev->next = r->next;
    r->next->prev = r->prev;
    ledger_table_schema_unlock(r->schema);
  }
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
  /* release the schema */{
    ledger_table_schema_free(r->schema);
  }
  return;
}

int ledger_table_row_attach
  (struct ledger_table_row* new_row, struct ledger_table_row* old_row)
{
  /* refuse attachment of rows of different shapes */
  if (new_row->schema != old_row->schema) return 0;
  ledger_table_schema_lock(new_row->schema);
  new_row->prev = old_row->prev;
  new_row->next = old_row;
  old_row->prev->next = new_row;
  old_row->prev = new_row;
  ledger_table_schema_unlock(new_row->schema);
  return 1;
}

struct ledger_table_row* ledger_table_row_acquire
  (struct ledger_table_row* r)
{
  return (struct ledger_table_row*)ledger_util_ref_acquire(r);
}

void ledger_table_drop_all_rows(struct ledger_table* table){
  if (table->root != NULL){
    struct ledger_table_mark* slow_mark = ledger_table_end(table);
    if (slow_mark == NULL){
      /* give up and */return;
    }
    ledger_table_mark_move(slow_mark, -1);
    while (slow_mark->row != table->root){
      ledger_table_drop_row(slow_mark);
    }
    ledger_table_mark_free(slow_mark);
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
    new_schema->lock_tf = 0;
    new_schema->outdated_tf = 0;
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
  /*assert(sch->lock_tf == 0);*/
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

void ledger_table_schema_lock(struct ledger_table_schema const* t){
  ((struct ledger_table_schema*)t)->lock_tf += 1;
  return;
}

void ledger_table_schema_lock2
  (struct ledger_table_schema const* t, struct ledger_table_schema const* t2)
{
  unsigned char buf[sizeof(struct ledger_table_schema const*)];
  unsigned char buf2[sizeof(struct ledger_table_schema const*)];
  memcpy(buf,&t,sizeof(t));
  memcpy(buf2,&t2,sizeof(t2));
  /* create a fixed locking order to eliminate deadlocks */
  if (memcmp(buf,buf2,sizeof(t)) < 0){
    ledger_table_schema_lock(t);
    ledger_table_schema_lock(t2);
  } else {
    ledger_table_schema_lock(t2);
    ledger_table_schema_lock(t);
  }
  return;
}

void ledger_table_schema_unlock(struct ledger_table_schema const* t){
  ((struct ledger_table_schema*)t)->lock_tf -= 1;
  return;
}

int ledger_table_schema_outdate(struct ledger_table_schema* t){
  int result;
  ledger_table_schema_lock(t);
  if (t->outdated_tf == 0){
    result = 1;
  } else {
    result = 0;
    ledger_table_schema_unlock(t);
  }
  return result;
}

void ledger_table_schema_close(struct ledger_table_schema* t){
  t->outdated_tf = 1;
  ledger_table_schema_unlock(t);
  return;
}

int ledger_table_schema_is_outdated(struct ledger_table_schema const* t){
  int outdate;
  ledger_table_schema_lock(t);
  outdate = t->outdated_tf;
  ledger_table_schema_unlock(t);
  return outdate;
}

int ledger_table_add_row_sub(struct ledger_table_mark* mark){
  if (mark->mutable_flag){
    int result = 0;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_row *new_row = NULL;
    struct ledger_table_schema const* const schema = old_row->schema;
    if (ledger_table_schema_is_outdated(schema)){
      result = 0;
    } else do {
      /* allocate the row */
      new_row = ledger_table_row_new(schema);
      if (new_row == NULL) break;
      /* acquire a reference for this function */
      if (new_row != ledger_table_row_acquire(new_row)) break;
      /* attach the row */
      if (!ledger_table_row_attach(new_row, old_row))
        break;
      /* set mark to new row */
      ledger_table_mark_exchange(mark, new_row);
      /* drop the reference for this function */
      ledger_table_row_free(new_row);
      /* cache the new row count */{
        ledger_table_lock(mark->source);
        table->rows += 1;
        ledger_table_unlock(mark->source);
      }
      result = 1;
    } while (0);
    if (!result){
      ledger_table_row_free(new_row);
    }
    return result;
  } else return 0;
}

int ledger_table_drop_row_sub(struct ledger_table_mark* mark){
  if (mark->mutable_flag){
    int result;
    struct ledger_table* const table = (struct ledger_table *)mark->source;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    if (ledger_table_schema_is_outdated(schema)){
      result = 0;
    } else if (old_row->root_tf){
      /* don't allow it */;
      result = 0;
    } else /* remove the row */{
      /* move the mark */
      ledger_table_mark_exchange(mark, old_row->prev);
      /* free the row (NOTE also detaches) */
      ledger_table_row_free(old_row);
      /* cache the new row count */
      ledger_table_lock(mark->source);
      table->rows -= 1;
      ledger_table_unlock(mark->source);
      /* done */
      result = 1;
    }
    return result;
  } else return 0;
}

int ledger_table_fetch_string_sub
  (struct ledger_table_mark const* mark, int i, unsigned char* buf, int len)
{
  /* const or mutable accepted */{
    int result;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row->root_tf
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

int ledger_table_put_string_sub
  (struct ledger_table_mark const* mark, int i, unsigned char const* value)
{
  if (mark->mutable_flag){
    int result;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row->root_tf
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

int ledger_table_fetch_bignum_sub
  (struct ledger_table_mark const* mark, int i, struct ledger_bignum* n)
{
  /* const or mutable accepted */{
    int result;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row->root_tf
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

int ledger_table_put_bignum_sub
  ( struct ledger_table_mark const* mark, int i,
    struct ledger_bignum const* value)
{
  if (mark->mutable_flag){
    int result;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row->root_tf
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


int ledger_table_fetch_id_sub
  (struct ledger_table_mark const* mark, int i, int* n)
{
  /* const or mutable accepted */{
    int result;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row->root_tf
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

int ledger_table_put_id_sub
  ( struct ledger_table_mark const* mark, int i, int value)
{
  if (mark->mutable_flag){
    int result;
    struct ledger_table_row * const old_row = mark->row;
    struct ledger_table_schema const* const schema = old_row->schema;
    int const columns = ((schema!=NULL)?schema->columns:0);
    int const* const types = ((schema!=NULL)?schema->types:0);
    if (old_row->root_tf
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

void ledger_table_mark_exchange
  (struct ledger_table_mark *mark, struct ledger_table_row* new_row)
{
  ledger_table_row_acquire(new_row);
  ledger_table_row_free(mark->row);
  mark->row = new_row;
  return;
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
  int result;
  struct ledger_table_schema *a_schema;
  struct ledger_table_schema *b_schema;
  struct ledger_table_mark a_mark;
  struct ledger_table_mark b_mark;
  /* trivial tables */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    int a_rows, b_rows;
    /* lock and access A */{
      ledger_table_lock(a);
      a_rows = a->rows;
      a_schema = ledger_table_schema_acquire(a->schema);
      ledger_table_mark_init(a,a->root,0,&a_mark);
      ledger_table_unlock(a);
    }
    /* lock and access B */{
      ledger_table_lock(b);
      b_rows = b->rows;
      b_schema = ledger_table_schema_acquire(b->schema);
      ledger_table_mark_init(b,b->root,0,&b_mark);
      ledger_table_unlock(b);
    }
    result = 1;
    do {
      if (a_rows != b_rows){
        result = 0;
        break;
      }
      if (a->schema == NULL || b->schema == NULL){
        /* failed to acquire PRE_CONDITION schema */
        result = 0;
        break;
      }
      if (!ledger_table_schema_is_equal(a->schema, b->schema)){
        result = 0;
        break;
      }
    } while(0);
    ledger_table_schema_free(a_schema);
    ledger_table_schema_free(b_schema);
    if (!result){
      /* the marks will not be used later, so clear them now */
      ledger_table_mark_clear(&a_mark);
      ledger_table_mark_clear(&b_mark);
      return 0;
    }
  }
  /* compare row by row */do {
    result = 0;
    /* perform initial consistency check */{
      if (a_mark.row->root_tf)
        /* pass */;
      else if (!ledger_table_mark_add_one_checked(&a_mark))
        break;
      if (b_mark.row->root_tf)
        /* pass */;
      else if (!ledger_table_mark_add_one_checked(&b_mark))
        break;
    }
    result = 1;
    while (!(a_mark.row->root_tf || b_mark.row->root_tf)){
      int i;
      struct ledger_table_row *const a_row = a_mark.row;
      struct ledger_table_row *const b_row = b_mark.row;
      int const *const types = a_row->schema->types;
      int const columns = a_row->schema->columns;
      ledger_table_schema_lock2(a_row->schema, b_row->schema);
      /* perform field-wise comparison */
      for (i = 0; i < columns; ++i){
        switch (types[i]){
        case LEDGER_TABLE_ID:
          {
            if (a_row->data[i].item_id != b_row->data[i].item_id)
              result = 0;
          }break;
        case LEDGER_TABLE_BIGNUM:
          {
            if (ledger_bignum_compare(
                a_row->data[i].bignum, b_row->data[i].bignum) != 0)
              result = 0;
          }break;
        case LEDGER_TABLE_USTR:
          {
            if (ledger_util_ustrcmp(
                a_row->data[i].string, b_row->data[i].string) != 0)
              result = 0;
          }break;
        }
        if (!result) break;
      }
      ledger_table_schema_unlock(a_row->schema);
      ledger_table_schema_unlock(b_row->schema);
      if (!result) break;
      /* perform consistency check */{
        if (!ledger_table_mark_add_one_checked(&a_mark)){
          result = 0;
          break;
        }
        if (!ledger_table_mark_add_one_checked(&b_mark)){
          result = 0;
          break;
        }
      }
    }
  } while(0);
  ledger_table_mark_clear(&a_mark);
  ledger_table_mark_clear(&b_mark);
  return result;
}

struct ledger_table_mark* ledger_table_begin(struct ledger_table* t){
  struct ledger_table_mark* m;
  /* acquire the mark */
  ledger_table_lock(t);
  m = ledger_table_mark_new(t, t->root, 1);
  ledger_table_unlock(t);
  /* move the mark */
  ledger_table_mark_move(m, +1);
  return m;
}

struct ledger_table_mark* ledger_table_begin_c
  (struct ledger_table const* t)
{
  struct ledger_table_mark* m;
  /* acquire the mark */
  ledger_table_lock(t);
  m = ledger_table_mark_new(t, t->root, 0);
  ledger_table_unlock(t);
  /* move the mark */
  ledger_table_mark_move(m, +1);
  return m;
}

struct ledger_table_mark* ledger_table_end(struct ledger_table* t){
  struct ledger_table_mark* m;
  ledger_table_lock(t);
  m = ledger_table_mark_new(t, t->root, 1);
  ledger_table_unlock(t);
  return m;
}

struct ledger_table_mark* ledger_table_end_c(struct ledger_table const* t){
  struct ledger_table_mark* m;
  ledger_table_lock(t);
  m = ledger_table_mark_new(t, t->root, 0);
  ledger_table_unlock(t);
  return m;
}

int ledger_table_mark_is_equal
  (struct ledger_table_mark const* a, struct ledger_table_mark const* b)
{
  return (a->source == b->source &&  a->row == b->row);
}

void ledger_table_mark_free(struct ledger_table_mark* m){
  ledger_table_mark_clear(m);
  ledger_util_free(m);
}

int ledger_table_get_column_count(struct ledger_table const* t){
  int columns;
  ledger_table_lock(t);
  columns = t->schema != NULL ? t->schema->columns : 0;
  ledger_table_unlock(t);
  return columns;
}

int ledger_table_get_column_type(struct ledger_table const* t, int i){
  int typ;
  ledger_table_lock(t);
  /* get type value */{
    struct ledger_table_schema const* const s = t->schema;
    if (i < 0 || i >= s->columns) typ = 0;
    else typ = s->types[i];
  }
  ledger_table_unlock(t);
  return typ;
}

int ledger_table_set_column_types
  (struct ledger_table* t, int n, int const* types)
{
  struct ledger_table_schema *new_schema;
  struct ledger_table_row *new_root;
  struct ledger_table_schema *old_schema;
  struct ledger_table_row *old_root;
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
  /* allocate new root */{
    new_root = ledger_table_row_new(new_schema);
    if (new_root == NULL){
      ledger_table_schema_free(new_schema);
      return 0;
    } else new_root->root_tf = 1;
  }
  /* reset the rows */if (ledger_table_schema_outdate(t->schema)){
    ledger_table_drop_all_rows(t);
    ledger_table_schema_close(t->schema);
  } else {
    /* either the table is closing,
     * or someone else is changing the schema */
    ledger_table_row_free(new_root);
    ledger_table_schema_free(new_schema);
    return 0;
  }
  ledger_table_lock(t);
  /* exchange pointers */{
    old_root = t->root; t->root = new_root;
    old_schema = t->schema; t->schema = new_schema;
  }
  ledger_table_unlock(t);
  /* drop the old root */{
    ledger_table_row_free(old_root);
  }
  /* drop the old schema */{
    ledger_table_schema_free(old_schema);
  }
  return 1;
}

int ledger_table_count_rows(struct ledger_table const* t){
  int nrows;
  ledger_table_lock(t);
  nrows = t->rows;
  ledger_table_unlock(t);
  return nrows;
}

int ledger_table_add_row(struct ledger_table_mark* mark){
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_add_row_sub(mark);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}

int ledger_table_drop_row(struct ledger_table_mark* mark){
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_drop_row_sub(mark);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}


int ledger_table_fetch_string
  (struct ledger_table_mark const* mark, int i, unsigned char* buf, int len)
{
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_fetch_string_sub(mark, i, buf, len);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}


int ledger_table_put_string
  (struct ledger_table_mark const* mark, int i, unsigned char const* value)
{
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_put_string_sub(mark, i, value);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}


int ledger_table_fetch_bignum
  (struct ledger_table_mark const* mark, int i, struct ledger_bignum* n)
{
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_fetch_bignum_sub(mark, i, n);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}

int ledger_table_put_bignum
  ( struct ledger_table_mark const* mark, int i,
    struct ledger_bignum const* value)
{
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_put_bignum_sub(mark, i, value);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}


void ledger_table_mark_move(struct ledger_table_mark* m, int n){
  struct ledger_table_schema *const schema = m->row->schema;
  if (n == 0) return;
  ledger_table_schema_lock(schema);
  if (!ledger_table_schema_is_outdated(schema)){
    if (n > 0){
      int i;
      for (i = 0; i < n; ++i){
        ledger_table_mark_exchange(m, m->row->next);
      }
    } else if (n < 0){
      int i;
      for (i = 0; i > n; --i){
        ledger_table_mark_exchange(m, m->row->prev);
      }
    }
  }
  ledger_table_schema_unlock(schema);
  return;
}

int ledger_table_fetch_id
  (struct ledger_table_mark const* mark, int i, int* n)
{
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_fetch_id_sub(mark, i, n);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}


int ledger_table_put_id
  ( struct ledger_table_mark const* mark, int i, int value)
{
  int result;
  ledger_table_schema_lock(mark->row->schema);
  result = ledger_table_put_id_sub(mark, i, value);
  ledger_table_schema_unlock(mark->row->schema);
  return result;
}

int ledger_table_mark_get_type
  (struct ledger_table_mark const* m, int i)
{
  struct ledger_table_schema *const schema = m->row->schema;
  if (i < 0
  ||  i >= schema->columns)
    return 0;
  else return schema->types[i];
}

/* END   implementation */
