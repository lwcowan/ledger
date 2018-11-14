
#include "../src/base/table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int allocate_test(void);
static int trivial_equal_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { trivial_equal_test, "trivial_equal" }
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
