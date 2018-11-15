
#include "../src/base/table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int allocate_test(void);
static int trivial_equal_test(void);
static int trivial_mark_test(void);
static int set_schema_test(void);
static int add_row_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { trivial_equal_test, "trivial_equal" },
  { trivial_mark_test, "trivial_mark_test" },
  { set_schema_test, "set schema" },
  { add_row_test, "add row" }
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
      /* TODO check the row */
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