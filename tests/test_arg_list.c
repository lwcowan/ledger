
#include "../src/act/arg.h"
#include "../src/base/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static int allocate_test(void);
static int equal_test(void);
static int trivial_equal_test(void);
static int new_arg_resize_test(void);
static int new_arg_equal_test(void);
static int arg_parse_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { equal_test, "equal" },
  { trivial_equal_test, "trivial equal" },
  { new_arg_equal_test, "argument equal" },
  { new_arg_resize_test, "argument resize" },
  { arg_parse_test, "argument parse" }
};


int allocate_test(void){
  struct ledger_arg_list* ptr;
  ptr = ledger_arg_list_new();
  if (ptr == NULL) return 0;
  ledger_arg_list_free(ptr);
  return 1;
}

int equal_test(void){
  int result = 0;
  struct ledger_arg_list* ptr, * other_ptr;
  ptr = ledger_arg_list_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_arg_list_new();
  if (other_ptr == NULL){
    ledger_arg_list_free(ptr);
    return 0;
  } else do {
    if (!ledger_arg_list_is_equal(ptr,other_ptr)) break;
    if (!ledger_arg_list_is_equal(other_ptr,ptr)) break;
    result = 1;
  } while (0);
  ledger_arg_list_free(ptr);
  ledger_arg_list_free(other_ptr);
  return result;
}

int trivial_equal_test(void){
  int result = 0;
  struct ledger_arg_list* ptr;
  ptr = ledger_arg_list_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_arg_list_is_equal(ptr,NULL)) break;
    if (!ledger_arg_list_is_equal(NULL,NULL)) break;
    if (!ledger_arg_list_is_equal(ptr,ptr)) break;
    if (ledger_arg_list_is_equal(NULL,ptr)) break;
    result = 1;
  } while (0);
  ledger_arg_list_free(ptr);
  return result;
}

int new_arg_resize_test(void){
  int result = 0;
  struct ledger_arg_list* ptr;
  ptr = ledger_arg_list_new();
  if (ptr == NULL) return 0;
  else do {
    int ok, i;
    if (ledger_arg_list_get_count(ptr) != 0) break;
    ok = ledger_arg_list_set_count(ptr,5);
    if (!ok) break;
    for (i = 0; i < 5; ++i){
      char const* l = ledger_arg_list_get(ptr,i);
      if (l != ledger_arg_list_get(ptr,i)) break;
      if (l != NULL) break;
    }
    if (i < 5) break;
    if (ledger_arg_list_get(ptr,5) != NULL) break;
    ok = ledger_arg_list_set(ptr, 1, "one");
    if (!ok) break;
    ok = ledger_arg_list_set(ptr, 3, "three");
    if (!ok) break;
    ok = ledger_arg_list_set_count(ptr,2);
    if (!ok) break;
    ok = ledger_arg_list_set_count(ptr,4);
    if (!ok) break;
    if (ledger_arg_list_get_count(ptr) != 4) break;
    for (i = 0; i < 10; ++i){
      char const* l = ledger_arg_list_get(ptr,i);
      if (i == 1){
        if (l == NULL) break;
        if (ledger_util_ustrcmp(
          (unsigned char const*)l,
          (unsigned char const*)"one") != 0) break;
      } else if (i < 4){
        if (l != NULL) break;
      } else {
        if (l != NULL) break;
      }
    }
    if (i < 10) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)ledger_arg_list_get(ptr,1),
        (unsigned char const*)"one") != 0)
      break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)ledger_arg_list_get(ptr,3),
        (unsigned char const*)"three") == 0)
      break;
    result = 1;
  } while (0);
  ledger_arg_list_free(ptr);
  return result;
}

int new_arg_equal_test(void){
  int result = 0;
  struct ledger_arg_list* ptr, * other_ptr;
  ptr = ledger_arg_list_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_arg_list_new();
  if (other_ptr == NULL){
    ledger_arg_list_free(ptr);
    return 0;
  } else do {
    int ok;
    char const* description =
      "some long string";
    char const* name =
      "short";
    /* different argument counts */
    ok = ledger_arg_list_set_count(ptr,1);
    if (!ok) break;
    if (ledger_arg_list_is_equal(ptr,other_ptr)) break;
    if (ledger_arg_list_is_equal(other_ptr,ptr)) break;
    /* different argument counts */
    ok = ledger_arg_list_set_count(other_ptr,2);
    if (!ok) break;
    if (ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (ledger_arg_list_is_equal(ptr,other_ptr)) break;
    /* same argument count */
    ok = ledger_arg_list_set_count(ptr,2);
    if (!ok) break;
    if (!ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (!ledger_arg_list_is_equal(ptr,other_ptr)) break;
    /* argument modification */{
      if (!ledger_arg_list_set(ptr,0,description)) break;
    }
    if (ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (ledger_arg_list_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      if (!ledger_arg_list_set(other_ptr,1,description)) break;
    }
    if (ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (ledger_arg_list_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      if (!ledger_arg_list_set(other_ptr,1,name)) break;
    }
    if (ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (ledger_arg_list_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      if (!ledger_arg_list_set(ptr,1,name)) break;
    }
    if (ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (ledger_arg_list_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      if (!ledger_arg_list_set(other_ptr,0,description)) break;
    }
    if (!ledger_arg_list_is_equal(other_ptr,ptr)) break;
    if (!ledger_arg_list_is_equal(ptr,other_ptr)) break;
    result = 1;
  } while (0);
  ledger_arg_list_free(ptr);
  ledger_arg_list_free(other_ptr);
  return result;
}


int arg_parse_test(void){
  int result = 0;
  struct ledger_arg_list* ptr;
  ptr = ledger_arg_list_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_arg_list_get_count(ptr) != 0) break;
    if (!ledger_arg_list_parse(ptr,"this that theother")) break;
    /* check */{
      if (ledger_arg_list_get_count(ptr) != 4) break;
      if (ledger_arg_list_get(ptr,0) == NULL) break;
      if (strcmp(ledger_arg_list_get(ptr,0),"this") != 0) break;
      if (ledger_arg_list_get(ptr,1) == NULL) break;
      if (strcmp(ledger_arg_list_get(ptr,1),"that") != 0) break;
      if (ledger_arg_list_get(ptr,2) == NULL) break;
      if (strcmp(ledger_arg_list_get(ptr,2),"theother") != 0) break;
      if (ledger_arg_list_get(ptr,3) != NULL) break;
    }
    if (!ledger_arg_list_parse(ptr,"\"the other\" this \"that \"")) break;
    /* check */{
      if (ledger_arg_list_get_count(ptr) != 4) break;
      if (ledger_arg_list_get(ptr,0) == NULL) break;
      if (strcmp(ledger_arg_list_get(ptr,0),"the other") != 0) break;
      if (ledger_arg_list_get(ptr,1) == NULL) break;
      if (strcmp(ledger_arg_list_get(ptr,1),"this") != 0) break;
      if (ledger_arg_list_get(ptr,2) == NULL) break;
      if (strcmp(ledger_arg_list_get(ptr,2),"that ") != 0) break;
      if (ledger_arg_list_get(ptr,3) != NULL) break;
    }
    result = 1;
  } while (0);
  ledger_arg_list_free(ptr);
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
