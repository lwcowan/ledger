
#include "../src/lua/luaext.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int allocate_test(char const* );

struct test_struct {
  int (*fn)(char const* );
  char const* name;
};

struct test_struct test_array[] = {
  { allocate_test, "Lua startup" }
};


int allocate_test(char const* name){
  (void)name;
  struct ledger_lua* ptr;
  ptr = ledger_lua_new();
  if (ptr == NULL) return 0;
  ledger_lua_close(ptr);
  return 1;
}


int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  char *use_filename;
  if (argc < 2){
    fprintf(stderr,"usage: test_luaext (path_to_tmp_file)\n");
    return EXIT_FAILURE;
  }
  use_filename = argv[1];
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)(use_filename))?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
