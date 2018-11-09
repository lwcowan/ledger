
#include "../src/base/book.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int allocate_test(void);
static int description_test(void);
static int null_description_test(void);
static int notes_test(void);
static int null_notes_test(void);
static int equal_test(void);
static int trivial_equal_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { description_test, "description" },
  { null_description_test, "null_description" },
  { notes_test, "notes" },
  { null_notes_test, "null_notes" },
  { equal_test, "equal" },
  { trivial_equal_test, "trivial_equal" }
};

int allocate_test(void){
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  ledger_book_free(ptr);
  return 1;
}
int description_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    char const* description = "new description";
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    if (ledger_book_set_description
        (ptr, (unsigned char const*)description) == 0)
      break;
    if (ledger_book_get_description(ptr) == NULL) break;
    if (strcmp((char const*)ledger_book_get_description(ptr),
        description) != 0)
      break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int equal_test(void){
  int result = 0;
  struct ledger_book* ptr, * other_ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_book_new();
  if (other_ptr == NULL){
    ledger_book_free(ptr);
    return 0;
  } else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"new description";
    unsigned char const* description2 =
      (unsigned char const*)"other description";
    /* different descriptions */
    ok = ledger_book_set_description(ptr,description);
    if (!ok) break;
    ok = ledger_book_set_description(other_ptr,description2);
    if (!ok) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    /* same descriptions */
    ok = ledger_book_set_description(other_ptr,description);
    if (!ok) break;
    if (!ledger_book_is_equal(ptr,other_ptr)) break;
    if (!ledger_book_is_equal(other_ptr,ptr)) break;
    /* null note versus non-empty note */
    ok = ledger_book_set_notes(other_ptr,description);
    if (!ok) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    /* different notes */
    ok = ledger_book_set_notes(ptr,description2);
    if (!ok) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* same notes */
    ok = ledger_book_set_notes(other_ptr,description2);
    if (!ok) break;
    if (!ledger_book_is_equal(other_ptr,ptr)) break;
    if (!ledger_book_is_equal(ptr,other_ptr)) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  ledger_book_free(other_ptr);
  return result;
}
int trivial_equal_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_book_is_equal(ptr,NULL)) break;
    if (!ledger_book_is_equal(NULL,NULL)) break;
    if (!ledger_book_is_equal(ptr,ptr)) break;
    if (ledger_book_is_equal(NULL,ptr)) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int null_description_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    char const* description = NULL;
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    if (ledger_book_set_description
        (ptr, (unsigned char const*)description) == 0)
      break;
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int notes_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    char const* notes = "new notes";
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    if (ledger_book_set_notes
        (ptr, (unsigned char const*)notes) == 0)
      break;
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) == NULL) break;
    if (strcmp((char const*)ledger_book_get_notes(ptr),
        notes) != 0)
      break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int null_notes_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    char const* notes = NULL;
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    if (ledger_book_set_notes
        (ptr, (unsigned char const*)notes) == 0)
      break;
    if (ledger_book_get_description(ptr) != NULL) break;
    if (ledger_book_get_notes(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
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
