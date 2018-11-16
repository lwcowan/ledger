
#include "../src/base/journal.h"
#include "../src/base/table.h"
#include "../src/base/entry.h"
#include "../src/base/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static int allocate_test(void);
static int allocate_table_test(void);
static int description_test(void);
static int null_description_test(void);
static int name_test(void);
static int id_test(void);
static int null_name_test(void);
static int equal_test(void);
static int trivial_equal_test(void);
static int alloc_id_test(void);
static int resume_alloc_id_test(void);
static int alloc_max_id_test(void);
static int new_entry_resize_test(void);
static int new_entry_equal_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { allocate_table_test, "table pre-allocation" },
  { description_test, "description" },
  { null_description_test, "null_description" },
  { name_test, "name" },
  { null_name_test, "null_name" },
  { id_test, "id" },
  { equal_test, "equal" },
  { trivial_equal_test, "trivial_equal" },
  { new_entry_equal_test, "entry_equal" },
  { alloc_id_test, "alloc_id" },
  { alloc_max_id_test, "alloc max id" },
  { resume_alloc_id_test, "resume_alloc_id" },
  { new_entry_resize_test, "entry resize" }
};

int allocate_test(void){
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  ledger_journal_free(ptr);
  return 1;
}

int allocate_table_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_journal_get_table(ptr) == NULL) break;
    if (ledger_journal_get_table_c(ptr) == NULL) break;
    /* check the table */{
      struct ledger_table const* table = ledger_journal_get_table_c(ptr);
      if (ledger_table_get_column_count(table) != 6) break;
      if (ledger_table_get_column_type(table,0) != 1) break;
      if (ledger_table_get_column_type(table,1) != 1) break;
      if (ledger_table_get_column_type(table,2) != 1) break;
      if (ledger_table_get_column_type(table,3) != 2) break;
      if (ledger_table_get_column_type(table,4) != 3) break;
      if (ledger_table_get_column_type(table,5) != 3) break;
    }
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}

int description_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    char const* description = "new description";
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    if (ledger_journal_set_description
        (ptr, (unsigned char const*)description) == 0)
      break;
    if (ledger_journal_get_description(ptr) == NULL) break;
    if (strcmp((char const*)ledger_journal_get_description(ptr),
        description) != 0)
      break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int equal_test(void){
  int result = 0;
  struct ledger_journal* ptr, * other_ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_journal_new();
  if (other_ptr == NULL){
    ledger_journal_free(ptr);
    return 0;
  } else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"new description";
    unsigned char const* description2 =
      (unsigned char const*)"other description";
    /* different descriptions */
    ok = ledger_journal_set_description(ptr,description);
    if (!ok) break;
    ok = ledger_journal_set_description(other_ptr,description2);
    if (!ok) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    /* same descriptions */
    ok = ledger_journal_set_description(other_ptr,description);
    if (!ok) break;
    if (!ledger_journal_is_equal(ptr,other_ptr)) break;
    if (!ledger_journal_is_equal(other_ptr,ptr)) break;
    /* null note versus non-empty note */
    ok = ledger_journal_set_name(other_ptr,description);
    if (!ok) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    /* different name */
    ok = ledger_journal_set_name(ptr,description2);
    if (!ok) break;
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    /* same name */
    ok = ledger_journal_set_name(other_ptr,description2);
    if (!ok) break;
    if (!ledger_journal_is_equal(other_ptr,ptr)) break;
    if (!ledger_journal_is_equal(ptr,other_ptr)) break;
    /* add a row */{
      struct ledger_table *const table = ledger_journal_get_table(ptr);
      struct ledger_table_mark *mark;
      ok = 0;
      do {
        mark = ledger_table_begin(table);
        if (mark == NULL) break;
        if (!ledger_table_add_row(mark))
          break;
        ok = 1;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  ledger_journal_free(other_ptr);
  return result;
}
int trivial_equal_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_journal_is_equal(ptr,NULL)) break;
    if (!ledger_journal_is_equal(NULL,NULL)) break;
    if (!ledger_journal_is_equal(ptr,ptr)) break;
    if (ledger_journal_is_equal(NULL,ptr)) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int null_description_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    char const* description = NULL;
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    if (ledger_journal_set_description
        (ptr, (unsigned char const*)description) == 0)
      break;
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int name_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    char const* name = "new name";
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    if (ledger_journal_set_name
        (ptr, (unsigned char const*)name) == 0)
      break;
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) == NULL) break;
    if (strcmp((char const*)ledger_journal_get_name(ptr),
        name) != 0)
      break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int id_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    if (ledger_journal_get_id(ptr) != -1) break;
    ledger_journal_set_id(ptr, 80);
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    if (ledger_journal_get_id(ptr) != 80) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int null_name_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    char const* name = NULL;
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    if (ledger_journal_set_name
        (ptr, (unsigned char const*)name) == 0)
      break;
    if (ledger_journal_get_description(ptr) != NULL) break;
    if (ledger_journal_get_name(ptr) != NULL) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}

int alloc_id_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_journal_get_sequence(ptr) != 0) break;
    if (ledger_journal_alloc_id(ptr) != 0) break;
    if (ledger_journal_alloc_id(ptr) != 1) break;
    if (ledger_journal_get_sequence(ptr) != 2) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int resume_alloc_id_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    if (!ledger_journal_set_sequence(ptr,57)) break;
    if (ledger_journal_alloc_id(ptr) != 57) break;
    if (ledger_journal_alloc_id(ptr) != 58) break;
    if (ledger_journal_get_sequence(ptr) != 59) break;
    if (ledger_journal_set_sequence(ptr,-45)) break;
    if (ledger_journal_alloc_id(ptr) != 59) break;
    if (!ledger_journal_set_sequence(ptr,0)) break;
    if (ledger_journal_get_sequence(ptr) != 0) break;
    if (ledger_journal_alloc_id(ptr) != 0) break;
    if (ledger_journal_get_sequence(ptr) != 1) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int alloc_max_id_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    if (!ledger_journal_set_sequence(ptr,INT_MAX-1)) break;
    if (ledger_journal_alloc_id(ptr) != (INT_MAX-1)) break;
    if (ledger_journal_alloc_id(ptr) >= 0) break;
    if (ledger_journal_get_sequence(ptr) != INT_MAX) break;
    if (!ledger_journal_set_sequence(ptr,0)) break;
    if (ledger_journal_alloc_id(ptr) != 0) break;
    if (ledger_journal_get_sequence(ptr) != 1) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int new_entry_resize_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    int ok, i;
    if (ledger_journal_get_entry_count(ptr) != 0) break;
    if (!ledger_journal_set_sequence(ptr,50)) break;
    ok = ledger_journal_set_entry_count(ptr,5);
    if (!ok) break;
    for (i = 0; i < 5; ++i){
      struct ledger_entry const* l = ledger_journal_get_entry_c(ptr,i);
      if (l != ledger_journal_get_entry(ptr,i)) break;
      if (l == NULL) break;
      if (ledger_entry_get_id(l) != i+50) break;
    }
    if (i < 5) break;
    if (ledger_journal_get_entry_c(ptr,5) != NULL) break;
    ok = ledger_entry_set_name
        (ledger_journal_get_entry(ptr,1), (unsigned char const*)"one");
    if (!ok) break;
    ok = ledger_entry_set_name
        (ledger_journal_get_entry(ptr,3), (unsigned char const*)"three");
    if (!ok) break;
    ok = ledger_journal_set_entry_count(ptr,2);
    if (!ok) break;
    ok = ledger_journal_set_entry_count(ptr,4);
    if (!ok) break;
    if (ledger_journal_get_entry_count(ptr) != 4) break;
    for (i = 0; i < 10; ++i){
      struct ledger_entry const* l = ledger_journal_get_entry_c(ptr,i);
      if (i < 2){
        if (l == NULL) break;
        if (ledger_entry_get_id(l) != i+50) break;
      } else if (i < 4){
        if (l == NULL) break;
        if (ledger_entry_get_id(l) != 55+i-2) break;
      } else {
        if (l != NULL) break;
      }
    }
    if (i < 10) break;
    if (ledger_util_ustrcmp(
          ledger_entry_get_name(ledger_journal_get_entry_c(ptr,1)),
          (unsigned char const*)"one")
        != 0)
      break;
    if (ledger_util_ustrcmp(
          ledger_entry_get_name(ledger_journal_get_entry_c(ptr,3)),
          (unsigned char const*)"three")
        == 0)
      break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}
int new_entry_equal_test(void){
  int result = 0;
  struct ledger_journal* ptr, * other_ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_journal_new();
  if (other_ptr == NULL){
    ledger_journal_free(ptr);
    return 0;
  } else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    /* different ledger counts */
    ok = ledger_journal_set_entry_count(ptr,1);
    if (!ok) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    /* different ledger counts */
    ok = ledger_journal_set_entry_count(other_ptr,2);
    if (!ok) break;
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    /* same ledger count */
    ok = ledger_journal_set_entry_count(ptr,2);
    if (!ok) break;
    if (!ledger_journal_is_equal(other_ptr,ptr)) break;
    if (!ledger_journal_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,0);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, description))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(other_ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, description))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(other_ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, name))
        break;
    }
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
      if (!ledger_entry_set_description(new_entry, name))
        break;
    }
    if (ledger_journal_is_equal(other_ptr,ptr)) break;
    if (ledger_journal_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(other_ptr,0);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, description))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    if (!ledger_journal_is_equal(other_ptr,ptr)) break;
    if (!ledger_journal_is_equal(ptr,other_ptr)) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  ledger_journal_free(other_ptr);
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
