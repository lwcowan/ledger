
#include "../src/base/book.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "../src/base/ledger.h"
#include "../src/base/util.h"
#include "../src/base/journal.h"

static int allocate_test(void);
static int description_test(void);
static int null_description_test(void);
static int notes_test(void);
static int null_notes_test(void);
static int equal_test(void);
static int trivial_equal_test(void);
static int alloc_id_test(void);
static int resume_alloc_id_test(void);
static int alloc_max_id_test(void);
static int new_ledger_resize_test(void);
static int new_ledger_equal_test(void);
static int new_journal_resize_test(void);
static int new_journal_equal_test(void);

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
  { trivial_equal_test, "trivial_equal" },
  { new_ledger_equal_test, "ledger_equal" },
  { alloc_id_test, "alloc_id" },
  { alloc_max_id_test, "alloc max id" },
  { resume_alloc_id_test, "resume_alloc_id" },
  { new_ledger_resize_test, "ledger resize" },
  { new_journal_equal_test, "journal equal" },
  { new_journal_resize_test, "journal resize" }
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
int new_ledger_equal_test(void){
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
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    /* different ledger counts */
    ok = ledger_book_set_ledger_count(ptr,1);
    if (!ok) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    /* different ledger counts */
    ok = ledger_book_set_ledger_count(other_ptr,2);
    if (!ok) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* same ledger count */
    ok = ledger_book_set_ledger_count(ptr,2);
    if (!ok) break;
    if (!ledger_book_is_equal(other_ptr,ptr)) break;
    if (!ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,0);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, description))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(other_ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, description))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(other_ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
      if (!ledger_ledger_set_description(new_ledger, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(other_ptr,0);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, description))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
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
int alloc_id_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_book_get_sequence(ptr) != 0) break;
    if (ledger_book_alloc_id(ptr) != 0) break;
    if (ledger_book_alloc_id(ptr) != 1) break;
    if (ledger_book_get_sequence(ptr) != 2) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int resume_alloc_id_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    if (!ledger_book_set_sequence(ptr,57)) break;
    if (ledger_book_alloc_id(ptr) != 57) break;
    if (ledger_book_alloc_id(ptr) != 58) break;
    if (ledger_book_get_sequence(ptr) != 59) break;
    if (ledger_book_set_sequence(ptr,-45)) break;
    if (ledger_book_alloc_id(ptr) != 59) break;
    if (!ledger_book_set_sequence(ptr,0)) break;
    if (ledger_book_get_sequence(ptr) != 0) break;
    if (ledger_book_alloc_id(ptr) != 0) break;
    if (ledger_book_get_sequence(ptr) != 1) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int alloc_max_id_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    if (!ledger_book_set_sequence(ptr,INT_MAX-1)) break;
    if (ledger_book_alloc_id(ptr) != (INT_MAX-1)) break;
    if (ledger_book_alloc_id(ptr) >= 0) break;
    if (ledger_book_get_sequence(ptr) != INT_MAX) break;
    if (!ledger_book_set_sequence(ptr,0)) break;
    if (ledger_book_alloc_id(ptr) != 0) break;
    if (ledger_book_get_sequence(ptr) != 1) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int new_ledger_resize_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    int ok, i;
    if (ledger_book_get_ledger_count(ptr) != 0) break;
    if (!ledger_book_set_sequence(ptr,50)) break;
    ok = ledger_book_set_ledger_count(ptr,5);
    if (!ok) break;
    for (i = 0; i < 5; ++i){
      struct ledger_ledger const* l = ledger_book_get_ledger_c(ptr,i);
      if (l != ledger_book_get_ledger(ptr,i)) break;
      if (l == NULL) break;
      if (ledger_ledger_get_id(l) != i+50) break;
    }
    if (i < 5) break;
    if (ledger_book_get_ledger_c(ptr,5) != NULL) break;
    ok = ledger_ledger_set_name
        (ledger_book_get_ledger(ptr,1), (unsigned char const*)"one");
    if (!ok) break;
    ok = ledger_ledger_set_name
        (ledger_book_get_ledger(ptr,3), (unsigned char const*)"three");
    if (!ok) break;
    ok = ledger_book_set_ledger_count(ptr,2);
    if (!ok) break;
    ok = ledger_book_set_ledger_count(ptr,4);
    if (!ok) break;
    if (ledger_book_get_ledger_count(ptr) != 4) break;
    for (i = 0; i < 10; ++i){
      struct ledger_ledger const* l = ledger_book_get_ledger_c(ptr,i);
      if (i < 2){
        if (l == NULL) break;
        if (ledger_ledger_get_id(l) != i+50) break;
      } else if (i < 4){
        if (l == NULL) break;
        if (ledger_ledger_get_id(l) != 55+i-2) break;
      } else {
        if (l != NULL) break;
      }
    }
    if (i < 10) break;
    if (ledger_util_ustrcmp(
          ledger_ledger_get_name(ledger_book_get_ledger_c(ptr,1)),
          (unsigned char const*)"one")
        != 0)
      break;
    if (ledger_util_ustrcmp(
          ledger_ledger_get_name(ledger_book_get_ledger_c(ptr,3)),
          (unsigned char const*)"three")
        == 0)
      break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}

int new_journal_resize_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    int ok, i;
    if (ledger_book_get_journal_count(ptr) != 0) break;
    if (ledger_book_get_ledger_count(ptr) != 0) break;
    if (!ledger_book_set_sequence(ptr,50)) break;
    ok = ledger_book_set_journal_count(ptr,5);
    if (!ok) break;
    for (i = 0; i < 5; ++i){
      struct ledger_journal const* l = ledger_book_get_journal_c(ptr,i);
      if (l != ledger_book_get_journal(ptr,i)) break;
      if (l == NULL) break;
      if (ledger_journal_get_id(l) != i+50) break;
    }
    if (i < 5) break;
    if (ledger_book_get_ledger_count(ptr) != 0) break;
    if (ledger_book_get_journal_c(ptr,5) != NULL) break;
    ok = ledger_journal_set_name
        (ledger_book_get_journal(ptr,1), (unsigned char const*)"one");
    if (!ok) break;
    ok = ledger_journal_set_name
        (ledger_book_get_journal(ptr,3), (unsigned char const*)"three");
    if (!ok) break;
    ok = ledger_book_set_journal_count(ptr,2);
    if (!ok) break;
    if (ledger_book_get_ledger_count(ptr) != 0) break;
    ok = ledger_book_set_journal_count(ptr,4);
    if (!ok) break;
    if (ledger_book_get_ledger_count(ptr) != 0) break;
    if (ledger_book_get_journal_count(ptr) != 4) break;
    for (i = 0; i < 10; ++i){
      struct ledger_journal const* l = ledger_book_get_journal_c(ptr,i);
      if (i < 2){
        if (l == NULL) break;
        if (ledger_journal_get_id(l) != i+50) break;
      } else if (i < 4){
        if (l == NULL) break;
        if (ledger_journal_get_id(l) != 55+i-2) break;
      } else {
        if (l != NULL) break;
      }
    }
    if (i < 10) break;
    if (ledger_util_ustrcmp(
          ledger_journal_get_name(ledger_book_get_journal_c(ptr,1)),
          (unsigned char const*)"one")
        != 0)
      break;
    if (ledger_util_ustrcmp(
          ledger_journal_get_name(ledger_book_get_journal_c(ptr,3)),
          (unsigned char const*)"three")
        == 0)
      break;
    if (ledger_book_get_ledger_count(ptr) != 0) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}
int new_journal_equal_test(void){
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
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    /* different ledger counts */
    ok = ledger_book_set_journal_count(ptr,1);
    if (!ok) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    /* different ledger counts */
    ok = ledger_book_set_journal_count(other_ptr,2);
    if (!ok) break;
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* same ledger count */
    ok = ledger_book_set_journal_count(ptr,2);
    if (!ok) break;
    if (!ledger_book_is_equal(other_ptr,ptr)) break;
    if (!ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,0);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, description))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(other_ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, description))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(other_ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
      if (!ledger_journal_set_description(new_journal, name))
        break;
    }
    if (ledger_book_is_equal(other_ptr,ptr)) break;
    if (ledger_book_is_equal(ptr,other_ptr)) break;
    /* ledger modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(other_ptr,0);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, description))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    if (!ledger_book_is_equal(other_ptr,ptr)) break;
    if (!ledger_book_is_equal(ptr,other_ptr)) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  ledger_book_free(other_ptr);
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
