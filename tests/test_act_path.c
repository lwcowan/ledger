
#include "../src/act/path.h"
#include "../src/base/book.h"
#include "../src/base/ledger.h"
#include "../src/base/journal.h"
#include "../src/base/account.h"
#include "../src/base/entry.h"
#include "../src/base/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static int root_path_test(void);
static int book_path_test(void);
static int quick_ledger_path_test(void);
static int quick_journal_path_test(void);
static int account_path_test(void);
static int book_path_render_test(void);
static int quick_ledger_render_test(void);
static int quick_journal_render_test(void);
static int account_render_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { root_path_test, "root path" },
  { book_path_test, "book path" },
  { quick_ledger_path_test, "ledger path" },
  { quick_journal_path_test, "journal path" },
  { account_path_test, "account path" },
  { book_path_render_test, "book render path" },
  { quick_ledger_render_test, "ledger render path" },
  { quick_journal_render_test, "journal render path" },
  { account_render_test, "account render path" }
};

int root_path_test(void){
  struct ledger_act_path path = ledger_act_path_root();
  if (path.path[0] != -1) return 0;
  if (path.path[1] != -1) return 0;
  if (path.len != 0) return 0;
  if (path.typ != 1) return 0;
  return 1;
}

int quick_ledger_path_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    struct ledger_act_path const root_path = ledger_act_path_root();
    struct ledger_act_path new_path;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    /* prepare the book */{
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
        ledger_ok =
          ledger_ledger_set_name(ledger, (unsigned char const*)"food");
        if (!ledger_ok) break;
        ledger_ok = ledger_ledger_set_description(ledger, text);
        if (!ledger_ok) break;
        ok = 1;
      } while (0);
      if (!ok) break;
    }
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger:food", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 2) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger#98", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 2) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger@2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger#2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger:2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger:98", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger#food", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger@0", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 2) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)".", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 2) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"..", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"../ledger:food/../ledger#99", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 2) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}

int book_path_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    struct ledger_act_path const root_path = ledger_act_path_root();
    struct ledger_act_path new_path;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/./", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/../", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)".", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"..", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}


int quick_journal_path_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    struct ledger_act_path const root_path = ledger_act_path_root();
    struct ledger_act_path new_path;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    /* prepare the book */{
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
        ledger_ok =
          ledger_journal_set_name(journal, (unsigned char const*)"food");
        if (!ledger_ok) break;
        ledger_ok = ledger_journal_set_description(journal, text);
        if (!ledger_ok) break;
        ok = 1;
      } while (0);
      if (!ok) break;
    }
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal:food", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal#98", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal@2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal#2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal:2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal:98", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal#food", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal@0", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)".", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"..", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"../journal:food/../journal#99", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}


int account_path_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    struct ledger_act_path const root_path = ledger_act_path_root();
    struct ledger_act_path new_path;
    /* prepare the book */{
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
        ledger_ok = ledger_journal_set_name
          (journal, (unsigned char const*)"recipe");
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
          entry_ok = ledger_entry_set_name
            (entry, (unsigned char const*)"add rice");
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
        ledger_ok = ledger_ledger_set_name
          (ledger, (unsigned char const*)"food");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"pasta");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"tofu");
          if (!account_ok) break;
          account_ok = ledger_account_set_description
            (account, (unsigned char const*)"yummy!");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"cash");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"receivables");
          if (!account_ok) break;
          account_ok = ledger_account_set_description
            (account, (unsigned char const*)"waiting...");
          if (!account_ok) break;
          ledger_ok = 1;
        }
        if (!ledger_ok) break;
        ok = 1;
      } while (0);
      if (!ok) break;
    }
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"/ledger:food/account:pasta",
        root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != 0) break;
    if (new_path.len != 2) break;
    if (new_path.typ != 3) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"/ledger:food/account@1",
        root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != 1) break;
    if (new_path.len != 2) break;
    if (new_path.typ != 3) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"/journal:recipe/entry:add rice/",
        root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != 2) break;
    if (new_path.len != 2) break;
    if (new_path.typ != 5) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal@2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger#2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal:2", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/ledger:98", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal#food", root_path, &ok);
    if (ok) break;
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)"/journal@0", root_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)".", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 0) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 1) break;
    if (new_path.typ != 4) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"..", new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != -1) break;
    if (new_path.path[1] != -1) break;
    if (new_path.len != 0) break;
    if (new_path.typ != 1) break;
    new_path = ledger_act_path_compute
      ( book, (unsigned char const*)"../ledger:food/../journal#99/entry#3",
        new_path, &ok);
    if (!ok) break;
    if (new_path.path[0] != 1) break;
    if (new_path.path[1] != 3) break;
    if (new_path.len != 2) break;
    if (new_path.typ != 5) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}

int book_path_render_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    unsigned char path_buffer[256];
    struct ledger_act_path const old_path = ledger_act_path_root();
    if (ledger_act_path_render
        (NULL, 0, old_path, book) != 1)
      break;
    if (ledger_act_path_render
        (path_buffer, sizeof(path_buffer), old_path, book) != 1)
      break;
    if (ledger_util_ustrcmp(path_buffer,
        (unsigned char const*)"/") != 0)
      break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}

int quick_ledger_render_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    unsigned char path_buffer[256];
    struct ledger_act_path old_path;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    /* prepare the book */{
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
        ledger_ok =
          ledger_ledger_set_name(ledger, (unsigned char const*)"food");
        if (!ledger_ok) break;
        ledger_ok = ledger_ledger_set_description(ledger, text);
        if (!ledger_ok) break;
        ok = 1;
      } while (0);
      if (!ok) break;
    }
    /* compose path */{
      old_path.path[0] = 1;
      old_path.path[1] = -1;
      old_path.len = 1;
      old_path.typ = LEDGER_ACT_PATH_LEDGER;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 12)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 12)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/ledger:food") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 0;
      old_path.path[1] = -1;
      old_path.len = 1;
      old_path.typ = LEDGER_ACT_PATH_LEDGER;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 10)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 10)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/ledger#98") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 2;
      old_path.path[1] = -1;
      old_path.len = 1;
      old_path.typ = LEDGER_ACT_PATH_LEDGER;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 9)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 9)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/ledger@2") != 0)
        break;
    }
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}


int quick_journal_render_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    unsigned char path_buffer[256];
    struct ledger_act_path old_path;
    unsigned char const *text = (unsigned char const*)"text note";
    unsigned char const *note = (unsigned char const*)"note text";
    /* prepare the book */{
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
        ledger_ok =
          ledger_journal_set_name(journal, (unsigned char const*)"food");
        if (!ledger_ok) break;
        ledger_ok = ledger_journal_set_description(journal, text);
        if (!ledger_ok) break;
        ok = 1;
      } while (0);
      if (!ok) break;
    }
    /* compose path */{
      old_path.path[0] = 1;
      old_path.path[1] = -1;
      old_path.len = 1;
      old_path.typ = LEDGER_ACT_PATH_JOURNAL;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 13)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 13)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/journal:food") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 0;
      old_path.path[1] = -1;
      old_path.len = 1;
      old_path.typ = LEDGER_ACT_PATH_JOURNAL;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 11)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 11)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/journal#98") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 2;
      old_path.path[1] = -1;
      old_path.len = 1;
      old_path.typ = LEDGER_ACT_PATH_JOURNAL;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 10)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 10)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/journal@2") != 0)
        break;
    }
    result = 1;
  } while (0);
  ledger_book_free(book);
  return result;
}


int account_render_test(void){
  int result = 0;
  struct ledger_book* book;
  book = ledger_book_new();
  if (book == NULL){
    return 0;
  } else do {
    int ok;
    unsigned char path_buffer[256];
    struct ledger_act_path old_path;
    /* prepare the book */{
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
        ledger_ok = ledger_journal_set_name
          (journal, (unsigned char const*)"recipe");
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
          entry_ok = ledger_entry_set_name
            (entry, (unsigned char const*)"add rice");
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
        ledger_ok = ledger_ledger_set_name
          (ledger, (unsigned char const*)"food");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"pasta");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"tofu");
          if (!account_ok) break;
          account_ok = ledger_account_set_description
            (account, (unsigned char const*)"yummy!");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"cash");
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
          account_ok = ledger_account_set_name
            (account, (unsigned char const*)"receivables");
          if (!account_ok) break;
          account_ok = ledger_account_set_description
            (account, (unsigned char const*)"waiting...");
          if (!account_ok) break;
          ledger_ok = 1;
        }
        if (!ledger_ok) break;
        ok = 1;
      } while (0);
      if (!ok) break;
    }
    /* compose path */{
      old_path.path[0] = 1;
      old_path.path[1] = 0;
      old_path.len = 2;
      old_path.typ = LEDGER_ACT_PATH_ACCOUNT;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 26)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 26)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/ledger:food/account:pasta") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 1;
      old_path.path[1] = 1;
      old_path.len = 2;
      old_path.typ = LEDGER_ACT_PATH_ACCOUNT;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 22)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 22)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/ledger:food/account#1") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 1;
      old_path.path[1] = 2;
      old_path.len = 2;
      old_path.typ = LEDGER_ACT_PATH_ENTRY;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 30)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 30)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/journal:recipe/entry:add rice") != 0)
        break;
    }
    /* compose path */{
      old_path.path[0] = 3;
      old_path.path[1] = 4;
      old_path.len = 2;
      old_path.typ = LEDGER_ACT_PATH_ENTRY;
      if (ledger_act_path_render
          (NULL, 0, old_path, book) != 18)
        break;
      if (ledger_act_path_render
          (path_buffer, sizeof(path_buffer), old_path, book) != 18)
        break;
      if (ledger_util_ustrcmp(path_buffer,
          (unsigned char const*)"/journal@3/entry@4") != 0)
        break;
    }
    result = 1;
  } while (0);
  ledger_book_free(book);
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
