
#include "../src/base/table.h"
#include "../src/base/util.h"
#include "../src/base/bignum.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int allocate_test(void);
static int trivial_equal_test(void);
static int trivial_mark_test(void);
static int set_schema_test(void);
static int add_row_test(void);
static int set_row_string_test(void);
static int switch_schema_test(void);
static int set_row_bignum_test(void);
static int move_mark_test(void);
static int nonzero_equal_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { trivial_equal_test, "trivial_equal" },
  { trivial_mark_test, "trivial_mark_test" },
  { set_schema_test, "set schema" },
  { add_row_test, "add row" },
  { switch_schema_test, "switch schema" },
  { set_row_string_test, "set row string" },
  { set_row_bignum_test, "set row bignum" },
  { move_mark_test, "mark move" },
  { nonzero_equal_test, "nonzero equal" }
};

int allocate_test(void){
  struct ledger_table* ptr;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  ledger_table_free(ptr);
  return 1;
}
int trivial_equal_test(void){
  int result = 0;
  struct ledger_table* ptr;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_table_is_equal(ptr,NULL)) break;
    if (!ledger_table_is_equal(NULL,NULL)) break;
    if (!ledger_table_is_equal(ptr,ptr)) break;
    if (ledger_table_is_equal(NULL,ptr)) break;
    result = 1;
  } while (0);
  ledger_table_free(ptr);
  return result;
}

int trivial_mark_test(void){
  int result = 0;
  struct ledger_table* ptr;
  struct ledger_table_mark* mark, * back_mark;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  else do {
    mark = ledger_table_begin(ptr);
    if (mark == NULL) break;
    back_mark = ledger_table_begin_c(ptr);
    if (back_mark == NULL) break;
    if (!ledger_table_mark_is_equal(mark, back_mark)) break;
    ledger_table_mark_free(back_mark);
    back_mark = NULL;
    back_mark = ledger_table_end(ptr);
    if (back_mark == NULL) break;
    if (!ledger_table_mark_is_equal(mark, back_mark)) break;
    result = 1;
  } while (0);
  ledger_table_mark_free(back_mark);
  ledger_table_mark_free(mark);
  ledger_table_free(ptr);
  return result;
}
int set_schema_test(void){
  int result = 0;
  struct ledger_table* ptr;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    int column_types[3] =
      { LEDGER_TABLE_ID, LEDGER_TABLE_USTR, LEDGER_TABLE_BIGNUM };
    ok = ledger_table_set_column_types(ptr,3,column_types);
    if (!ok) break;
    if (ledger_table_get_column_count(ptr) != 3) break;
    if (ledger_table_get_column_type(ptr,0) != 1) break;
    if (ledger_table_get_column_type(ptr,1) != 3) break;
    if (ledger_table_get_column_type(ptr,2) != 2) break;
    result = 1;
  } while (0);
  ledger_table_free(ptr);
  return result;
}

int add_row_test(void){
  int result = 0;
  struct ledger_table* ptr;
  struct ledger_table_mark* mark = NULL;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    int column_types[3] =
      { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
    ok = ledger_table_set_column_types(ptr,3,column_types);
    if (!ok) break;
    /* iterate from the start */{
      mark = ledger_table_begin(ptr);
      if (mark == NULL) break;
      ok = ledger_table_add_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 1) break;
      ok = ledger_table_drop_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_table_mark_free(mark);
  ledger_table_free(ptr);
  return result;
}

int switch_schema_test(void){
  int result = 0;
  struct ledger_table* ptr;
  struct ledger_table_mark* mark = NULL;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    int column_types[3] =
      { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
    ok = ledger_table_set_column_types(ptr,3,column_types);
    if (!ok) break;
    /* iterate from the start */{
      mark = ledger_table_begin(ptr);
      if (mark == NULL) break;
      ok = ledger_table_add_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 1) break;
      ok = ledger_table_set_column_types(ptr,2,column_types);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 0) break;
      ledger_table_mark_free(mark);
      mark = ledger_table_begin(ptr);
      ok = ledger_table_drop_row(mark);
      if (ok) break;
      if (ledger_table_count_rows(ptr) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_table_mark_free(mark);
  ledger_table_free(ptr);
  return result;
}

int set_row_string_test(void){
  int result = 0;
  struct ledger_table* ptr;
  struct ledger_table_mark* mark = NULL;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    int column_types[3] =
      { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
    ok = ledger_table_set_column_types(ptr,3,column_types);
    if (!ok) break;
    /* iterate from the start */{
      unsigned char const* numeric =
        (unsigned char const*)"034.56";
      mark = ledger_table_begin(ptr);
      if (mark == NULL) break;
      ok = ledger_table_add_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 1) break;
      /* set the row */{
        if (!ledger_table_put_string(mark, 0, numeric)) break;
        if (!ledger_table_put_string(mark, 1, numeric)) break;
        if (!ledger_table_put_string(mark, 2, numeric)) break;
      }
      /* check the row */{
        unsigned char buf[16];
        if (ledger_table_fetch_string(mark, 0, buf, sizeof(buf)) != 5)
          break;
        if (ledger_util_ustrcmp(buf,
            (unsigned char const*)"34.56") != 0)
          break;
        if (ledger_table_fetch_string(mark, 1, buf, sizeof(buf)) != 6)
          break;
        if (ledger_util_ustrcmp(buf,
            (unsigned char const*)"034.56") != 0)
          break;
        if (ledger_table_fetch_string(mark, 2, buf, sizeof(buf)) != 2)
          break;
        if (ledger_util_ustrcmp(buf,
            (unsigned char const*)"34") != 0)
          break;
      }
      ok = ledger_table_drop_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_table_mark_free(mark);
  ledger_table_free(ptr);
  return result;
}

int set_row_bignum_test(void){
  int result = 0;
  struct ledger_table* ptr;
  struct ledger_table_mark* mark = NULL;
  struct ledger_bignum* storage;
  struct ledger_bignum* numeric;
  storage = ledger_bignum_new();
  if (storage == NULL) return 0;
  numeric = ledger_bignum_new();
  if (numeric == NULL){
    ledger_bignum_free(storage);
    return 0;
  }
  if (!ledger_bignum_set_text(numeric,(unsigned char const*)"45.61",NULL)){
    ledger_bignum_free(storage);
    ledger_bignum_free(numeric);
    return 0;
  }
  ptr = ledger_table_new();
  if (ptr == NULL){
    ledger_bignum_free(storage);
    ledger_bignum_free(numeric);
    return 0;
  } else do {
    int ok;
    int column_types[3] =
      { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
    ok = ledger_table_set_column_types(ptr,3,column_types);
    if (!ok) break;
    /* iterate from the start */{
      mark = ledger_table_begin(ptr);
      if (mark == NULL) break;
      ok = ledger_table_add_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 1) break;
      /* set the row */{
        if (!ledger_table_put_bignum(mark, 0, numeric)) break;
        if (!ledger_table_put_bignum(mark, 1, numeric)) break;
        if (!ledger_table_put_bignum(mark, 2, numeric)) break;
      }
      ledger_table_mark_free(mark);
      mark = ledger_table_begin_c(ptr);
      if (mark == NULL) break;
      /* check the row */{
        if (!ledger_table_fetch_bignum(mark, 0, storage))
          break;
        if (ledger_bignum_compare(storage, numeric) != 0)
          break;
        if (!ledger_table_fetch_bignum(mark, 1, storage))
          break;
        if (ledger_bignum_compare(storage, numeric) != 0)
          break;
        if (!ledger_table_fetch_bignum(mark, 2, storage))
          break;
        /* bignum in, integer out, so should not be equal */
        if (ledger_bignum_compare(storage, numeric) >= 0)
          break;
        if (ledger_bignum_get_long(storage) != 45)
          break;
      }
      ledger_table_mark_free(mark);
      mark = ledger_table_begin(ptr);
      if (mark == NULL) break;
      ok = ledger_table_drop_row(mark);
      if (!ok) break;
      if (ledger_table_count_rows(ptr) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_table_mark_free(mark);
  ledger_table_free(ptr);
  ledger_bignum_free(storage);
  ledger_bignum_free(numeric);
  return result;
}

int move_mark_test(void){
  int result = 0;
  struct ledger_table* ptr;
  struct ledger_table_mark* mark = NULL;
  struct ledger_bignum* storage;
  struct ledger_bignum* numeric;
  storage = ledger_bignum_new();
  if (storage == NULL) return 0;
  numeric = ledger_bignum_new();
  if (numeric == NULL){
    ledger_bignum_free(storage);
    return 0;
  }
  if (!ledger_bignum_alloc(numeric,1,0)){
    ledger_bignum_free(storage);
    ledger_bignum_free(numeric);
    return 0;
  }
  ptr = ledger_table_new();
  if (ptr == NULL){
    ledger_bignum_free(storage);
    ledger_bignum_free(numeric);
    return 0;
  } else do {
    int ok;
    int column_types[1] = { LEDGER_TABLE_ID };
    ok = ledger_table_set_column_types(ptr,1,column_types);
    if (!ok) break;
    /* iterate from the start */{
      int const row_count = 5;
      int const count_offset = 35;
      int i;
      mark = ledger_table_begin(ptr);
      if (mark == NULL) break;
      for (i = 0; i < row_count; ++i){
        ok = ledger_table_add_row(mark);
        if (!ok) break;
        if (ledger_table_count_rows(ptr) != i+1) break;
        /* set the row */{
          if (!ledger_bignum_set_long(numeric, i+count_offset)) break;
          if (!ledger_table_put_bignum(mark, 0, numeric)) break;
        }
        ledger_table_mark_move(mark, +1);
      }
      if (i < row_count) break;
      ledger_table_mark_free(mark);
      mark = ledger_table_end(ptr);
      if (mark == NULL) break;
      for (i = row_count-1; i >= 0; --i){
        ledger_table_mark_move(mark, -1);
        if (!ledger_table_fetch_bignum(mark, 0, storage))
          break;
        if (ledger_bignum_get_long(storage) != i+count_offset)
          break;
      }
      if (i >= 0) break;
    }
    result = 1;
  } while (0);
  ledger_table_mark_free(mark);
  ledger_table_free(ptr);
  ledger_bignum_free(storage);
  ledger_bignum_free(numeric);
  return result;
}

int nonzero_equal_test(void){
  int result = 0;
  struct ledger_table* ptr, * other_ptr;
  ptr = ledger_table_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_table_new();
  if (other_ptr == NULL){
    ledger_table_free(ptr);
    return 0;
  }
  int column_types[3] =
    { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
  int other_column_types[3] =
    { LEDGER_TABLE_USTR, LEDGER_TABLE_ID, LEDGER_TABLE_BIGNUM };
  int const column_count = 3;
  do {
    int ok;
    if (!ledger_table_is_equal(ptr, other_ptr)) break;
    if (!ledger_table_is_equal(other_ptr, ptr)) break;
    /* set mismatched schemata */{
      if (!ledger_table_set_column_types(
          ptr, column_count, column_types)) break;
      if (ledger_table_is_equal(ptr, other_ptr)) break;
      if (ledger_table_is_equal(other_ptr, ptr)) break;
      if (!ledger_table_set_column_types(
          other_ptr, column_count, other_column_types))
        break;
      if (ledger_table_is_equal(ptr, other_ptr)) break;
      if (ledger_table_is_equal(other_ptr, ptr)) break;
    }
    /* set mismatched rows (one) */{
      ok = 0;
      struct ledger_table_mark* mark = NULL;
      do {
        int i, j;
        int const row_count = 3;
        mark = ledger_table_begin(ptr);
        if (mark == NULL) break;
        /* write the tables */{
          for (j = 0; j < row_count; ++j){
            if (!ledger_table_add_row(mark)) break;
            for (i = 0; i < column_count; ++i){
              if (!ledger_table_put_string(mark, i,
                  (unsigned char*)"099.99"))
                break;
            }
            if (i < column_count) break;
            ledger_table_mark_move(mark, +1);
          }
          if (j < row_count) break;
        }
        ok = 1;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    /* set mismatched rows (other) */{
      ok = 0;
      struct ledger_table_mark* mark = NULL;
      do {
        int i, j;
        int const row_count = 3;
        mark = ledger_table_begin(other_ptr);
        if (mark == NULL) break;
        /* write the tables */{
          for (j = 0; j < row_count; ++j){
            if (!ledger_table_add_row(mark)) break;
            for (i = 0; i < column_count; ++i){
              if (!ledger_table_put_string(mark, i,
                  (unsigned char*)"099.99"))
                break;
            }
            if (i < column_count) break;
            ledger_table_mark_move(mark, +1);
          }
          if (j < row_count) break;
        }
        ok = 1;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    if (ledger_table_is_equal(ptr, other_ptr)) break;
    if (ledger_table_is_equal(other_ptr, ptr)) break;
    /* set matched schemata */{
      if (!ledger_table_set_column_types(
          other_ptr, column_count, column_types))
        break;
      if (ledger_table_is_equal(ptr, other_ptr)) break;
      if (ledger_table_is_equal(other_ptr, ptr)) break;
    }
    /* set matched rows (other) */{
      ok = 0;
      struct ledger_table_mark* mark = NULL;
      do {
        int i, j;
        int const row_count = 3;
        mark = ledger_table_begin(other_ptr);
        if (mark == NULL) break;
        /* write the tables */{
          for (j = 0; j < row_count; ++j){
            if (!ledger_table_add_row(mark)) break;
            for (i = 0; i < column_count; ++i){
              if (!ledger_table_put_string(mark, i,
                  (unsigned char*)"099.99"))
                break;
            }
            if (i < column_count) break;
            ledger_table_mark_move(mark, +1);
          }
          if (j < row_count) break;
        }
        ok = 1;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    if (!ledger_table_is_equal(ptr, other_ptr)) break;
    if (!ledger_table_is_equal(other_ptr, ptr)) break;
    result = 1;
  } while (0);
  ledger_table_free(other_ptr);
  ledger_table_free(ptr);
  return result;
}



int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)())?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
