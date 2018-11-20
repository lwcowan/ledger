
#include "../src/act/commit.h"
#include "../src/act/transact.h"
#include "../src/base/book.h"
#include "../src/base/journal.h"
#include "../src/base/entry.h"
#include "../src/base/util.h"
#include "../src/base/table.h"
#include "../src/base/ledger.h"
#include "../src/base/account.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static int zero_commit_test(void);
static int nonzero_commit_test(void);


struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { zero_commit_test, "commit empty transaction" },
  { nonzero_commit_test, "commit non-empty transaction" }
};

int zero_commit_test(void){
  int result = 0;
  struct ledger_transaction* transaction;
  struct ledger_book* book;
  transaction = ledger_transaction_new();
  if (transaction == NULL){
    return 0;
  }
  book = ledger_book_new();
  if (book == NULL){
    ledger_transaction_free(transaction);
    return 0;
  } else do {
    int ok;
    unsigned char const* name =
            (unsigned char const*)"five";
    ledger_transaction_set_journal(transaction, 0);
    ok = ledger_transaction_set_name(transaction, name);
    if (!ok) break;
    ok = ledger_commit_transaction(book, transaction);
    if (ok) break;
    ok = ledger_book_set_journal_count(book, 1);
    if (!ok) break;
    ok = ledger_commit_transaction(book, transaction);
    if (!ok) break;
    /* confer with the journal */{
      struct ledger_journal const* journal =
        ledger_book_get_journal_c(book, 0);
      if (journal == NULL) break;
      if (ledger_journal_get_entry_count(journal) != 1)
        break;
      /* check the entry */{
        struct ledger_entry const* entry =
          ledger_journal_get_entry_c(journal, 0);
        if (entry == NULL) break;
        if (ledger_util_ustrcmp(ledger_entry_get_name(entry),
            name) != 0)
          break;
      }
    }
    result = 1;
  } while (0);
  ledger_transaction_free(transaction);
  ledger_book_free(book);
  return result;
}

int nonzero_commit_test(void){
  int result = 0;
  struct ledger_transaction* transaction;
  struct ledger_book* book;
  transaction = ledger_transaction_new();
  if (transaction == NULL){
    return 0;
  }
  book = ledger_book_new();
  if (book == NULL){
    ledger_transaction_free(transaction);
    return 0;
  } else do {
    int ok;
    unsigned char const* name =
            (unsigned char const*)"five";
    unsigned char const* date =
            (unsigned char const*)"2018-11-19T06:54:32Z";
    /* prepare the transaction */{
      struct ledger_table *const table =
          ledger_transaction_get_table(transaction);
      struct ledger_table_mark *const mark =
          ledger_table_begin(table);
      if (mark == NULL) break;
      else do {
        ledger_transaction_set_journal(transaction, 0);
        ok = ledger_transaction_set_name(transaction, name);
        if (!ok) break;
        ok = ledger_transaction_set_date(transaction, date);
        if (!ok) break;
        ok = ledger_table_add_row(mark);
        if (!ok) break;
        /* set target account */
        ok = ledger_table_put_string(mark, 2,
                      (unsigned char const*)"/ledger@0/account@0");
        if (!ok) break;
        /* set amount */
        ok = ledger_table_put_string(mark, 3,
                      (unsigned char const*)"12.34");
        if (!ok) break;
        /* set check */
        ok = ledger_table_put_string(mark, 4,
                      (unsigned char const*)"+100");
        if (!ok) break;
        ledger_table_mark_move(mark, +1);
        ok = ledger_table_add_row(mark);
        if (!ok) break;
        /* set target account */
        ok = ledger_table_put_string(mark, 2,
                      (unsigned char const*)"/ledger@0/account@1");
        if (!ok) break;
        /* set amount */
        ok = ledger_table_put_string(mark, 3,
                      (unsigned char const*)"-12.34");
        if (!ok) break;
        /* set check */
        ok = ledger_table_put_string(mark, 4, NULL);
        if (!ok) break;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    ok = ledger_commit_transaction(book, transaction);
    if (ok) break;
    ok = ledger_book_set_journal_count(book, 1);
    if (!ok) break;
    ok = ledger_commit_transaction(book, transaction);
    if (ok) break;
    ok = ledger_book_set_ledger_count(book, 1);
    if (!ok) break;
    ok = ledger_commit_transaction(book, transaction);
    if (ok) break;
    /* prepare the ledger */{
      struct ledger_ledger *const ledger =
        ledger_book_get_ledger(book, 0);
      ok = ledger_ledger_set_account_count(ledger, 2);
      if (!ok) break;
    }
    ok = ledger_commit_transaction(book, transaction);
    if (!ok) break;
    /* confer with the journal */{
      struct ledger_journal const* journal =
        ledger_book_get_journal_c(book, 0);
      if (journal == NULL) break;
      if (ledger_journal_get_entry_count(journal) != 1)
        break;
      /* check the entry */{
        struct ledger_entry const* entry =
          ledger_journal_get_entry_c(journal, 0);
        if (entry == NULL) break;
        if (ledger_util_ustrcmp(ledger_entry_get_name(entry),
            name) != 0)
          break;
        if (ledger_util_ustrcmp(ledger_entry_get_date(entry),
            date) != 0)
          break;
      }
      /* check the lines */{
        struct ledger_table const* journal_table =
          ledger_journal_get_table_c(journal);
        if (ledger_table_count_rows(journal_table) != 2)
          break;
        /* iterate through the rows */{
          struct ledger_table_mark* const mark =
            ledger_table_begin_c(journal_table);
          ok = 0;
          if (mark == NULL) break;
          else do {
            int value;
            if (!ledger_table_fetch_id(mark,0,&value)) break;
            if (value != 0) break;/* transaction identifier */
            if (!ledger_table_fetch_id(mark,1,&value)) break;
            if (value != 1) break;/* ledger identifier */
            if (!ledger_table_fetch_id(mark,2,&value)) break;
            if (value != 0) break;/* account identifier */
            if (!ledger_table_fetch_id(mark,3,&value)) break;
            if (value != +12) break;/* amount */
            if (!ledger_table_fetch_id(mark,4,&value)) break;
            if (value != +100) break;/* check value */
            ledger_table_mark_move(mark, +1);
            if (!ledger_table_fetch_id(mark,0,&value)) break;
            if (value != 0) break;/* transaction identifier */
            if (!ledger_table_fetch_id(mark,1,&value)) break;
            if (value != 1) break;/* ledger identifier */
            if (!ledger_table_fetch_id(mark,2,&value)) break;
            if (value != 1) break;/* account identifier */
            if (!ledger_table_fetch_id(mark,3,&value)) break;
            if (value != -12) break;/* amount */
            if (!ledger_table_fetch_id(mark,4,&value)) break;
            if (value != 0) break;/* check value */
            ok = 1;
          } while (0);
          if (!ok) break;
          ledger_table_mark_free(mark);
        }
      }
    }
    /* confer with the ledger */{
      struct ledger_ledger const* ledger =
        ledger_book_get_ledger_c(book, 0);
      if (ledger == NULL) break;
      /* inspect account@0 */{
        struct ledger_account const* account =
          ledger_ledger_get_account_c(ledger, 0);
        if (account == NULL) break;
        else {
          struct ledger_table const* account_table =
            ledger_account_get_table_c(account);
          if (ledger_table_count_rows(account_table) != 1)
            break;
          /* iterate through the rows */{
            struct ledger_table_mark* const mark =
              ledger_table_begin_c(account_table);
            ok = 0;
            if (mark == NULL) break;
            else do {
              int value;
              if (!ledger_table_fetch_id(mark,0,&value)) break;
              if (value != 0) break;/* journal identifier */
              if (!ledger_table_fetch_id(mark,1,&value)) break;
              if (value != 0) break;/* transaction identifier */
              if (!ledger_table_fetch_id(mark,2,&value)) break;
              if (value != +12) break;/* amount */
              if (!ledger_table_fetch_id(mark,3,&value)) break;
              if (value != +100) break;/* check */
              ok = 1;
            } while (0);
            if (!ok) break;
            ledger_table_mark_free(mark);
          }
        }
      }
      /* inspect account@1 */{
        struct ledger_account const* account =
          ledger_ledger_get_account_c(ledger, 1);
        if (account == NULL) break;
        else {
          struct ledger_table const* account_table =
            ledger_account_get_table_c(account);
          if (ledger_table_count_rows(account_table) != 1)
            break;
          /* iterate through the rows */{
            struct ledger_table_mark* const mark =
              ledger_table_begin_c(account_table);
            ok = 0;
            if (mark == NULL) break;
            else do {
              int value;
              if (!ledger_table_fetch_id(mark,0,&value)) break;
              if (value != 0) break;/* journal identifier */
              if (!ledger_table_fetch_id(mark,1,&value)) break;
              if (value != 0) break;/* transaction identifier */
              if (!ledger_table_fetch_id(mark,2,&value)) break;
              if (value != -12) break;/* amount */
              if (!ledger_table_fetch_id(mark,3,&value)) break;
              if (value != 0) break;/* check */
              ok = 1;
            } while (0);
            if (!ok) break;
            ledger_table_mark_free(mark);
          }
        }
      }
    }
    result = 1;
  } while (0);
  ledger_transaction_free(transaction);
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
