
#include "../src/base/book.h"
#include "../src/base/ledger.h"
#include "../src/base/account.h"
#include "../src/base/journal.h"
#include "../src/base/entry.h"
#include "../src/io/book.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int io_write_zero_test(char const* );
static int io_write_nonzero_test(char const* );
static int io_write_ledger_test(char const* );
static int io_write_journal_test(char const* );
static int io_write_account_test(char const* );
static int account_journal_test(char const* );
static int account_journal_entry_test(char const* );

struct test_struct {
  int (*fn)(char const* );
  char const* name;
};
struct test_struct test_array[] = {
  { io_write_zero_test, "i/o write zero" },
  { io_write_nonzero_test, "i/o write nonzero" },
  { io_write_ledger_test, "ledger writing" },
  { io_write_account_test, "account writing" },
  { io_write_journal_test, "journal writing" },
  { account_journal_test, "account and journal writing" },
  { account_journal_entry_test, "account and journal entry writing" }
};

int io_write_zero_test(char const* fn){
  int result = 0;
  struct ledger_book* ptr, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  ptr = ledger_book_new();
  if (ptr == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    ok = ledger_io_book_write(fn,ptr);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,ptr)) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  ledger_book_free(back_book);
  return result;
}
int io_write_nonzero_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    unsigned char const *note = (unsigned char const*)"note note";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}
int io_write_ledger_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_book_set_sequence(book, 98);
    if (!ok) break;
    ok = ledger_book_set_ledger_count(book, 2);
    if (!ok) break;
    /* encounter the ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,1);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_name(ledger, "food");
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_description(ledger, text);
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}
int io_write_account_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_book_set_sequence(book, 98);
    if (!ok) break;
    ok = ledger_book_set_ledger_count(book, 2);
    if (!ok) break;
    /* encounter the ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,1);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_name(ledger, "food");
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_description(ledger, text);
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_account_count(ledger, 3);
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,0);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "pasta");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,2);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "tofu");
        if (!account_ok) break;
        account_ok = ledger_account_set_description(account, "yummy!");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    /* encounter the other ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,0);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_account_count(ledger, 2);
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,1);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "cash");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,0);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "receivables");
        if (!account_ok) break;
        account_ok = ledger_account_set_description(account, "waiting...");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}
int io_write_journal_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_book_set_sequence(book, 98);
    if (!ok) break;
    ok = ledger_book_set_journal_count(book, 2);
    if (!ok) break;
    /* encounter the journal */do {
      int ledger_ok;
      struct ledger_journal* journal;
      ok = 0;
      journal = ledger_book_get_journal(book,1);
      if (journal == NULL) break;
      ledger_ok = ledger_journal_set_name(journal, "food");
      if (!ledger_ok) break;
      ledger_ok = ledger_journal_set_description(journal, text);
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}
int account_journal_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_book_set_sequence(book, 98);
    if (!ok) break;
    ok = ledger_book_set_journal_count(book, 2);
    if (!ok) break;
    /* encounter the journal */do {
      int ledger_ok;
      struct ledger_journal* journal;
      ok = 0;
      journal = ledger_book_get_journal(book,1);
      if (journal == NULL) break;
      ledger_ok = ledger_journal_set_name(journal, "food");
      if (!ledger_ok) break;
      ledger_ok = ledger_journal_set_description(journal, text);
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_book_set_ledger_count(book, 2);
    if (!ok) break;
    /* encounter the ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,1);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_name(ledger, "food");
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_description(ledger, text);
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_account_count(ledger, 3);
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,0);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "pasta");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,2);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "tofu");
        if (!account_ok) break;
        account_ok = ledger_account_set_description(account, "yummy!");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    /* encounter the other ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,0);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_account_count(ledger, 2);
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,1);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "cash");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,0);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "receivables");
        if (!account_ok) break;
        account_ok = ledger_account_set_description(account, "waiting...");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}

int account_journal_entry_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_book_set_sequence(book, 98);
    if (!ok) break;
    ok = ledger_book_set_journal_count(book, 2);
    if (!ok) break;
    /* encounter the journal */do {
      int ledger_ok;
      struct ledger_journal* journal;
      ok = 0;
      journal = ledger_book_get_journal(book,1);
      if (journal == NULL) break;
      ledger_ok = ledger_journal_set_name(journal, "recipe");
      if (!ledger_ok) break;
      ledger_ok = ledger_journal_set_description(journal, text);
      if (!ledger_ok) break;
      ledger_ok = ledger_journal_set_entry_count(journal, 5);
      if (!ledger_ok) break;
      /* encounter the entry */{
        int entry_ok;
        struct ledger_entry* entry;
        ledger_ok = 0;
        entry = ledger_journal_get_entry(journal,2);
        if (entry == NULL) break;
        entry_ok = ledger_entry_set_name(entry, "add rice");
        if (!entry_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_book_set_ledger_count(book, 2);
    if (!ok) break;
    /* encounter the ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,1);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_name(ledger, "food");
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_description(ledger, text);
      if (!ledger_ok) break;
      ledger_ok = ledger_ledger_set_account_count(ledger, 3);
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,0);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "pasta");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,2);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "tofu");
        if (!account_ok) break;
        account_ok = ledger_account_set_description(account, "yummy!");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    /* encounter the other ledger */do {
      int ledger_ok;
      struct ledger_ledger* ledger;
      ok = 0;
      ledger = ledger_book_get_ledger(book,0);
      if (ledger == NULL) break;
      ledger_ok = ledger_ledger_set_account_count(ledger, 2);
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,1);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "cash");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      /* encounter the account */{
        int account_ok;
        struct ledger_account* account;
        ledger_ok = 0;
        account = ledger_ledger_get_account(ledger,0);
        if (account == NULL) break;
        account_ok = ledger_account_set_name(account, "receivables");
        if (!account_ok) break;
        account_ok = ledger_account_set_description(account, "waiting...");
        if (!account_ok) break;
        ledger_ok = 1;
      }
      if (!ledger_ok) break;
      ok = 1;
    } while (0);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}



int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  char *use_filename;
  if (argc < 2){
    fprintf(stderr,"usage: test_io_book (path_to_tmp_file)\n");
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
