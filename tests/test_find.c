
#include "../src/base/find.h"
#include "../src/base/book.h"
#include "../src/base/ledger.h"
#include "../src/base/journal.h"
#include "../src/base/account.h"
#include "../src/base/entry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int find_ledger_name_test(void);
static int find_ledger_id_test(void);
static int find_journal_name_test(void);
static int find_journal_id_test(void);
static int find_account_name_test(void);
static int find_account_id_test(void);
static int find_entry_name_test(void);
static int find_entry_id_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { find_ledger_name_test, "find ledger by name" },
  { find_ledger_id_test, "find ledger by identifier" },
  { find_journal_name_test, "find journal by name" },
  { find_journal_id_test, "find journal by identifier" },
  { find_account_name_test, "find account by name" },
  { find_account_id_test, "find account by identifier" },
  { find_entry_name_test, "find entry by name" },
  { find_entry_id_test, "find entry by identifier" }
};


int find_ledger_name_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_book_set_sequence(ptr, 77)) break;
    /* different ledger counts */
    ok = ledger_book_set_ledger_count(ptr,2);
    if (!ok) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,0);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, description))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, name))
        break;
      if (!ledger_ledger_set_name(new_ledger, description))
        break;
    }
    /* find ledgers */
    if (ledger_find_ledger_by_name(ptr, description) != 1) break;
    if (ledger_find_ledger_by_name(ptr, name) != 0) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, NULL))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
    /* find ledgers */
    if (ledger_find_ledger_by_name(ptr, description) != -1) break;
    if (ledger_find_ledger_by_name(ptr, name) != 0) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}

int find_ledger_id_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_book_set_sequence(ptr, 77)) break;
    /* different ledger counts */
    ok = ledger_book_set_ledger_count(ptr,2);
    if (!ok) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,0);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, description))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, name))
        break;
      if (!ledger_ledger_set_name(new_ledger, description))
        break;
    }
    /* find ledgers */
    if (ledger_find_ledger_by_id(ptr, 78) != 1) break;
    if (ledger_find_ledger_by_id(ptr, 77) != 0) break;
    /* ledger modification */{
      struct ledger_ledger* new_ledger = ledger_book_get_ledger(ptr,1);
      if (new_ledger == NULL) break;
      if (!ledger_ledger_set_description(new_ledger, NULL))
        break;
      if (!ledger_ledger_set_name(new_ledger, name))
        break;
    }
    /* find ledgers */
    if (ledger_find_ledger_by_id(ptr, 76) != -1) break;
    if (ledger_find_ledger_by_id(ptr, 77) != 0) break;
    if (ledger_find_ledger_by_id(ptr, 78) != 1) break;
    if (ledger_find_ledger_by_id(ptr, 79) != -1) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}


int find_journal_name_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_book_set_sequence(ptr, 77)) break;
    /* different journal counts */
    ok = ledger_book_set_journal_count(ptr,2);
    if (!ok) break;
    /* journal modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,0);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, description))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    /* journal modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, name))
        break;
      if (!ledger_journal_set_name(new_journal, description))
        break;
    }
    /* find journals */
    if (ledger_find_journal_by_name(ptr, description) != 1) break;
    if (ledger_find_journal_by_name(ptr, name) != 0) break;
    /* journal modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, NULL))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    /* find journals */
    if (ledger_find_journal_by_name(ptr, description) != -1) break;
    if (ledger_find_journal_by_name(ptr, name) != 0) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}

int find_journal_id_test(void){
  int result = 0;
  struct ledger_book* ptr;
  ptr = ledger_book_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_book_set_sequence(ptr, 77)) break;
    /* different journal counts */
    ok = ledger_book_set_ledger_count(ptr,2);
    ok = ledger_book_set_journal_count(ptr,2);
    if (!ok) break;
    /* journal modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,0);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, description))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    /* journal modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, name))
        break;
      if (!ledger_journal_set_name(new_journal, description))
        break;
    }
    /* find journals */
    if (ledger_find_journal_by_id(ptr, 80) != 1) break;
    if (ledger_find_journal_by_id(ptr, 79) != 0) break;
    if (ledger_find_journal_by_id(ptr, 78) != -1) break;
    if (ledger_find_journal_by_id(ptr, 77) != -1) break;
    /* journal modification */{
      struct ledger_journal* new_journal = ledger_book_get_journal(ptr,1);
      if (new_journal == NULL) break;
      if (!ledger_journal_set_description(new_journal, NULL))
        break;
      if (!ledger_journal_set_name(new_journal, name))
        break;
    }
    /* find journals */
    if (ledger_find_journal_by_id(ptr, 76) != -1) break;
    if (ledger_find_journal_by_id(ptr, 77) != -1) break;
    if (ledger_find_journal_by_id(ptr, 78) != -1) break;
    if (ledger_find_journal_by_id(ptr, 79) != 0) break;
    if (ledger_find_journal_by_id(ptr, 80) != 1) break;
    if (ledger_find_journal_by_id(ptr, 81) != -1) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  return result;
}



int find_account_name_test(void){
  int result = 0;
  struct ledger_ledger* ptr;
  ptr = ledger_ledger_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_ledger_set_sequence(ptr, 77)) break;
    /* different account counts */
    ok = ledger_ledger_set_account_count(ptr,2);
    if (!ok) break;
    /* account modification */{
      struct ledger_account* new_account = ledger_ledger_get_account(ptr,0);
      if (new_account == NULL) break;
      if (!ledger_account_set_description(new_account, description))
        break;
      if (!ledger_account_set_name(new_account, name))
        break;
    }
    /* account modification */{
      struct ledger_account* new_account = ledger_ledger_get_account(ptr,1);
      if (new_account == NULL) break;
      if (!ledger_account_set_description(new_account, name))
        break;
      if (!ledger_account_set_name(new_account, description))
        break;
    }
    /* find accounts */
    if (ledger_find_account_by_name(ptr, description) != 1) break;
    if (ledger_find_account_by_name(ptr, name) != 0) break;
    /* account modification */{
      struct ledger_account* new_account = ledger_ledger_get_account(ptr,1);
      if (new_account == NULL) break;
      if (!ledger_account_set_description(new_account, NULL))
        break;
      if (!ledger_account_set_name(new_account, name))
        break;
    }
    /* find accounts */
    if (ledger_find_account_by_name(ptr, description) != -1) break;
    if (ledger_find_account_by_name(ptr, name) != 0) break;
    result = 1;
  } while (0);
  ledger_ledger_free(ptr);
  return result;
}

int find_account_id_test(void){
  int result = 0;
  struct ledger_ledger* ptr;
  ptr = ledger_ledger_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_ledger_set_sequence(ptr, 77)) break;
    /* different account counts */
    ok = ledger_ledger_set_account_count(ptr,2);
    if (!ok) break;
    ok = ledger_ledger_set_account_count(ptr,0);
    if (!ok) break;
    ok = ledger_ledger_set_account_count(ptr,2);
    if (!ok) break;
    /* account modification */{
      struct ledger_account* new_account = ledger_ledger_get_account(ptr,0);
      if (new_account == NULL) break;
      if (!ledger_account_set_description(new_account, description))
        break;
      if (!ledger_account_set_name(new_account, name))
        break;
    }
    /* account modification */{
      struct ledger_account* new_account = ledger_ledger_get_account(ptr,1);
      if (new_account == NULL) break;
      if (!ledger_account_set_description(new_account, name))
        break;
      if (!ledger_account_set_name(new_account, description))
        break;
    }
    /* find accounts */
    if (ledger_find_account_by_id(ptr, 80) != 1) break;
    if (ledger_find_account_by_id(ptr, 79) != 0) break;
    if (ledger_find_account_by_id(ptr, 78) != -1) break;
    if (ledger_find_account_by_id(ptr, 77) != -1) break;
    /* account modification */{
      struct ledger_account* new_account = ledger_ledger_get_account(ptr,1);
      if (new_account == NULL) break;
      if (!ledger_account_set_description(new_account, NULL))
        break;
      if (!ledger_account_set_name(new_account, name))
        break;
    }
    /* find accounts */
    if (ledger_find_account_by_id(ptr, 76) != -1) break;
    if (ledger_find_account_by_id(ptr, 77) != -1) break;
    if (ledger_find_account_by_id(ptr, 78) != -1) break;
    if (ledger_find_account_by_id(ptr, 79) != 0) break;
    if (ledger_find_account_by_id(ptr, 80) != 1) break;
    if (ledger_find_account_by_id(ptr, 81) != -1) break;
    result = 1;
  } while (0);
  ledger_ledger_free(ptr);
  return result;
}



int find_entry_name_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_journal_set_sequence(ptr, 77)) break;
    /* different entry counts */
    ok = ledger_journal_set_entry_count(ptr,2);
    if (!ok) break;
    /* entry modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,0);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, description))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    /* entry modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, name))
        break;
      if (!ledger_entry_set_name(new_entry, description))
        break;
    }
    /* find entries */
    if (ledger_find_entry_by_name(ptr, description) != 1) break;
    if (ledger_find_entry_by_name(ptr, name) != 0) break;
    /* entry modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, NULL))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    /* find entries */
    if (ledger_find_entry_by_name(ptr, description) != -1) break;
    if (ledger_find_entry_by_name(ptr, name) != 0) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
  return result;
}

int find_entry_id_test(void){
  int result = 0;
  struct ledger_journal* ptr;
  ptr = ledger_journal_new();
  if (ptr == NULL) return 0;
  else do {
    int ok;
    unsigned char const* description =
      (unsigned char const*)"some long string";
    unsigned char const* name =
      (unsigned char const*)"short";
    if (!ledger_journal_set_sequence(ptr, 77)) break;
    ok = ledger_journal_set_entry_count(ptr,3);
    if (!ok) break;
    ok = ledger_journal_set_entry_count(ptr,1);
    if (!ok) break;
    /* different entry counts */
    ok = ledger_journal_set_entry_count(ptr,2);
    if (!ok) break;
    /* entry modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,0);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, description))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    /* entry modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, name))
        break;
      if (!ledger_entry_set_name(new_entry, description))
        break;
    }
    /* find entries */
    if (ledger_find_entry_by_id(ptr, 80) != 1) break;
    if (ledger_find_entry_by_id(ptr, 79) != -1) break;
    if (ledger_find_entry_by_id(ptr, 78) != -1) break;
    if (ledger_find_entry_by_id(ptr, 77) != 0) break;
    /* entry modification */{
      struct ledger_entry* new_entry = ledger_journal_get_entry(ptr,1);
      if (new_entry == NULL) break;
      if (!ledger_entry_set_description(new_entry, NULL))
        break;
      if (!ledger_entry_set_name(new_entry, name))
        break;
    }
    /* find entries */
    if (ledger_find_entry_by_id(ptr, 76) != -1) break;
    if (ledger_find_entry_by_id(ptr, 77) != 0) break;
    if (ledger_find_entry_by_id(ptr, 78) != -1) break;
    if (ledger_find_entry_by_id(ptr, 79) != -1) break;
    if (ledger_find_entry_by_id(ptr, 80) != 1) break;
    if (ledger_find_entry_by_id(ptr, 81) != -1) break;
    result = 1;
  } while (0);
  ledger_journal_free(ptr);
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
