
#include "../src/base/entry.h"
#include "../src/base/table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int allocate_test(void);
static int description_test(void);
static int null_description_test(void);
static int name_test(void);
static int id_test(void);
static int null_name_test(void);
static int equal_test(void);
static int trivial_equal_test(void);
static int date_test(void);
static int null_date_test(void);
static int date_equal_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { description_test, "description" },
  { null_description_test, "null_description" },
  { name_test, "name" },
  { null_name_test, "null_name" },
  { id_test, "id" },
  { equal_test, "equal" },
  { trivial_equal_test, "trivial_equal" },
  { date_test, "date" },
  { null_date_test, "null_date" },
  { date_equal_test, "date_equal" }
};


int allocate_test(void){
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  ledger_entry_free(ptr);
  return 1;
}

int description_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    char const* description = "new description";
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_set_description
        (ptr, (unsigned char const*)description) == 0)
      break;
    if (ledger_entry_get_description(ptr) == NULL) break;
    if (strcmp((char const*)ledger_entry_get_description(ptr),
        description) != 0)
      break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int equal_test(void){
  int result = 0;
  struct ledger_entry* ptr, * other_ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_entry_new();
  if (other_ptr == NULL){
    ledger_entry_free(ptr);
    return 0;
  } else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"new description";
    unsigned char const* description2 =
      (unsigned char const*)"other description";
    /* different descriptions */
    ok = ledger_entry_set_description(ptr,description);
    if (!ok) break;
    ok = ledger_entry_set_description(other_ptr,description2);
    if (!ok) break;
    if (ledger_entry_is_equal(ptr,other_ptr)) break;
    if (ledger_entry_is_equal(other_ptr,ptr)) break;
    /* same descriptions */
    ok = ledger_entry_set_description(other_ptr,description);
    if (!ok) break;
    if (!ledger_entry_is_equal(ptr,other_ptr)) break;
    if (!ledger_entry_is_equal(other_ptr,ptr)) break;
    /* null note versus non-empty note */
    ok = ledger_entry_set_name(other_ptr,description);
    if (!ok) break;
    if (ledger_entry_is_equal(ptr,other_ptr)) break;
    if (ledger_entry_is_equal(other_ptr,ptr)) break;
    /* different name */
    ok = ledger_entry_set_name(ptr,description2);
    if (!ok) break;
    if (ledger_entry_is_equal(other_ptr,ptr)) break;
    if (ledger_entry_is_equal(ptr,other_ptr)) break;
    /* same name */
    ok = ledger_entry_set_name(other_ptr,description2);
    if (!ok) break;
    if (!ledger_entry_is_equal(other_ptr,ptr)) break;
    if (!ledger_entry_is_equal(ptr,other_ptr)) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  ledger_entry_free(other_ptr);
  return result;
}

int trivial_equal_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_entry_is_equal(ptr,NULL)) break;
    if (!ledger_entry_is_equal(NULL,NULL)) break;
    if (!ledger_entry_is_equal(ptr,ptr)) break;
    if (ledger_entry_is_equal(NULL,ptr)) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int null_description_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    char const* description = NULL;
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_set_description
        (ptr, (unsigned char const*)description) == 0)
      break;
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int name_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    char const* name = "new name";
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_set_name
        (ptr, (unsigned char const*)name) == 0)
      break;
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) == NULL) break;
    if (strcmp((char const*)ledger_entry_get_name(ptr),
        name) != 0)
      break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int id_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_get_id(ptr) != -1) break;
    ledger_entry_set_id(ptr, 80);
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_get_id(ptr) != 80) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int null_name_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    char const* name = NULL;
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_set_name
        (ptr, (unsigned char const*)name) == 0)
      break;
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int date_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    char const* date = "2018-11-15T23:18:40Z";
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_get_date(ptr) != NULL) break;
    if (ledger_entry_set_date
        (ptr, (unsigned char const*)date) == 0)
      break;
    if (ledger_entry_get_description(ptr) != NULL) break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_get_date(ptr) == NULL) break;
    if (strcmp((char const*)ledger_entry_get_date(ptr),
        date) != 0)
      break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int null_date_test(void){
  int result = 0;
  struct ledger_entry* ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  else do {
    char const* date = NULL;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_get_date(ptr) != NULL) break;
    if (ledger_entry_set_date
        (ptr, (unsigned char const*)date) == 0)
      break;
    if (ledger_entry_get_name(ptr) != NULL) break;
    if (ledger_entry_get_date(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  return result;
}

int date_equal_test(void){
  int result = 0;
  struct ledger_entry* ptr, * other_ptr;
  ptr = ledger_entry_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_entry_new();
  if (other_ptr == NULL){
    ledger_entry_free(ptr);
    return 0;
  } else do {
    int ok;
    unsigned char const* name =
      (unsigned char const*)"new description";
    unsigned char const* name2 =
      (unsigned char const*)"other description";
    char const* date = "2018-11-15T23:18:40Z";
    char const* date2 = "2016-05-24T05:49:01Z";
    /* different names */
    ok = ledger_entry_set_name(ptr,name);
    if (!ok) break;
    ok = ledger_entry_set_name(other_ptr,name2);
    if (!ok) break;
    if (ledger_entry_is_equal(ptr,other_ptr)) break;
    if (ledger_entry_is_equal(other_ptr,ptr)) break;
    /* same names */
    ok = ledger_entry_set_name(other_ptr,name);
    if (!ok) break;
    if (!ledger_entry_is_equal(ptr,other_ptr)) break;
    if (!ledger_entry_is_equal(other_ptr,ptr)) break;
    /* null date versus non-empty date */
    ok = ledger_entry_set_date(other_ptr,(unsigned char const*)date);
    if (!ok) break;
    if (ledger_entry_is_equal(ptr,other_ptr)) break;
    if (ledger_entry_is_equal(other_ptr,ptr)) break;
    /* different date */
    ok = ledger_entry_set_date(ptr,(unsigned char const*)date2);
    if (!ok) break;
    if (ledger_entry_is_equal(other_ptr,ptr)) break;
    if (ledger_entry_is_equal(ptr,other_ptr)) break;
    /* same date */
    ok = ledger_entry_set_date(other_ptr,(unsigned char const*)date2);
    if (!ok) break;
    if (!ledger_entry_is_equal(other_ptr,ptr)) break;
    if (!ledger_entry_is_equal(ptr,other_ptr)) break;
    result = 1;
  } while (0);
  ledger_entry_free(ptr);
  ledger_entry_free(other_ptr);
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
