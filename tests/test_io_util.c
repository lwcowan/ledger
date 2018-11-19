
#include "../src/io/util.h"
#include "../src/base/util.h"
#include "../src/base/bignum.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int construct_name_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { construct_name_test, "construct name" }
};


int construct_name_test(void){
  int result = 0;
  struct ledger_bignum* num;
  num = ledger_bignum_new();
  if (num == NULL) return 0;
  else do {
    char buffer[36];
    int name_length;
    memset(buffer,37,sizeof(buffer));
    name_length = ledger_io_util_construct_name(buffer, sizeof(buffer), num,
        "this%ithat%ix", 99, -123);
    if (name_length != 15) break;
    if (buffer[15] != 0) break;
    if (strcmp(buffer,"this99that-123x") != 0) break;
    result = 1;
  } while (0);
  ledger_bignum_free(num);
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
