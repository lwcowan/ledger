
#include "commit.h"
#include "transact.h"
#include "path.h"
#include "../base/util.h"
#include "../base/book.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/journal.h"
#include "../base/entry.h"
#include "../base/bignum.h"
#include "../base/table.h"
#include <limits.h>


struct ledger_commit_pair {
  struct ledger_table_mark* ledger;
  struct ledger_table_mark* journal;
};

struct ledger_commit {
  struct ledger_commit_pair *pairs;
  int pair_count;
  int entry_id;
  int entry_index;
};

/*
 * Verify the identifiers in a transaction.
 * - book the source book
 * - act the transaction to verify
 * @return one on success, zero otherwise
 */
int ledger_commit_verify
  (struct ledger_book const* book, struct ledger_transaction* act);

/*
 * Acquire a journal entry descriptor for this transaction.
 * - commit commission structure
 * - book the source book
 * - act the transaction to verify
 * @return one on success, zero otherwise
 */
int ledger_commit_acquire_entry
  ( struct ledger_commit* commit, struct ledger_book* book,
    struct ledger_transaction const* act);

/*
 * Acquire and write journal lines and account lines for this transaction.
 * - commit commission structure
 * - book the source book
 * - act the transaction to apply
 * @return one on success, zero otherwise
 */
int ledger_commit_acquire_lines
  ( struct ledger_commit* commit, struct ledger_book* book,
    struct ledger_transaction const* act);

/*
 * Delete all marks.
 * - commit the commit structure to clear
 */
void ledger_commit_clear(struct ledger_commit* commit);

/*
 * Reverse a (partial) commit.
 * - commit the commit structure to reverse
 * - book the source book
 */
void ledger_commit_rollback
  ( struct ledger_commit* commit, struct ledger_book* book,
    struct ledger_transaction const* act);

/*
 * Initialize a commit structure.
 * - commit the commit structure to initialize
 */
void ledger_commit_init(struct ledger_commit* commit);

/*
 * Allocate space for mark pairs.
 * - commit the commit structure to modify
 * - act transaction to effect
 * @return one on success, zero otherwise
 */
int ledger_commit_allocate_pairs
  ( struct ledger_commit* commit, struct ledger_transaction const* act);


/* BEGIN static implementation */

void ledger_commit_init(struct ledger_commit* commit){
  commit->entry_id = -1;
  commit->entry_index = -1;
  commit->pairs = NULL;
  commit->pair_count = 0;
  return ;
}

int ledger_commit_verify
  (struct ledger_book const* book, struct ledger_transaction* act)
{
  int result = 0;
  /* first pass: resolve account names */{
    struct ledger_table_mark* act_end;
    struct ledger_table_mark* act_mark;
    /* acquire transaction table marks */{
      struct ledger_table* const table = ledger_transaction_get_table(act);
      act_mark = ledger_table_begin(table);
      act_end = ledger_table_end(table);
      if (act_mark != NULL && act_end != NULL){
        while (!ledger_table_mark_is_equal(act_mark, act_end)){
          struct ledger_act_path account_path;
          unsigned char account_path_string[256];
          int ok;
          ok = ledger_table_fetch_string
            (act_mark, 2, account_path_string, sizeof(account_path_string));
          if (ok < 0 || ok >= 256) break;
          if (ok > 0){
            /* actual string to resolve */
            account_path = ledger_act_path_compute
              ( book, account_path_string, ledger_act_path_root(), &ok);
            if (!ok) break;
            if (account_path.typ != LEDGER_ACT_PATH_ACCOUNT)
              break;
          } else {
            ok = ledger_table_fetch_id(act_mark, 0, &account_path.path[0]);
            if (!ok) break;
            ok = ledger_table_fetch_id(act_mark, 1, &account_path.path[1]);
            if (!ok) break;
          }
          /* confirm account path */{
            struct ledger_ledger const* check_ledger =
              ledger_book_get_ledger_c(book, account_path.path[0]);
            if (check_ledger == NULL) break;
            else {
              struct ledger_account const* check_account =
                ledger_ledger_get_account_c
                    (check_ledger, account_path.path[1]);
              if (check_account == NULL) break;
            }
          }
          /* post new path */{
            ok = ledger_table_put_id(act_mark, 0, account_path.path[0]);
            if (!ok) break;
            ok = ledger_table_put_id(act_mark, 1, account_path.path[1]);
            if (!ok) break;
          }
          ledger_table_mark_move(act_mark,+1);
        }
        result = ledger_table_mark_is_equal(act_mark, act_end);
      } else result = 0;
      ledger_table_mark_free(act_mark);
      ledger_table_mark_free(act_end);
    }
  }
  return result;
}

int ledger_commit_acquire_entry
  ( struct ledger_commit* commit, struct ledger_book* book,
    struct ledger_transaction const* act)
{
  int entry_index;
  int entry_id;
  int result;
  struct ledger_journal *active_journal =
    ledger_book_get_journal(book, ledger_transaction_get_journal(act));
  if (active_journal == NULL){
    return 0;
  }
  entry_index = ledger_journal_get_entry_count(active_journal);
  result = ledger_journal_set_entry_count(active_journal, entry_index+1);
  if (result == 0) return 0;
  else {
    int ok = 0;
    struct ledger_entry* entry =
      ledger_journal_get_entry(active_journal, entry_index);
    entry_id = ledger_entry_get_id(entry);
    /* apply properties to the entry */do{
      if (!ledger_entry_set_description
          (entry, ledger_transaction_get_description(act)))
        break;
      if (!ledger_entry_set_name
          (entry, ledger_transaction_get_name(act)))
        break;
      if (!ledger_entry_set_date
          (entry, ledger_transaction_get_date(act)))
        break;
      ok = 1;
    } while (0);
    if (!ok){
      result = ledger_journal_set_entry_count(active_journal, entry_index);
      if (!result){
        ledger_entry_set_id(entry, -1);
      }
      result = 0;
    } else result = 1;
  }
  commit->entry_index = entry_index;
  commit->entry_id = entry_id;
  return result;
}

int ledger_commit_acquire_lines
  ( struct ledger_commit* commit, struct ledger_book* book,
    struct ledger_transaction const* act)
{
  int result;
  int const journal_index = ledger_transaction_get_journal(act);
  struct ledger_bignum* tmp_bignum;
  char const* date = ledger_transaction_get_date(act);
  struct ledger_journal* const active_journal =
    ledger_book_get_journal(book, journal_index);
  struct ledger_table* const active_j_table =
    ledger_journal_get_table(active_journal);
  int const journal_id = ledger_journal_get_id(active_journal);
  /* allocate the pairs array */{
    result = ledger_commit_allocate_pairs(commit, act);
    if (result == 0) return 0;
  }
  tmp_bignum = ledger_bignum_new();
  if (tmp_bignum == NULL) return 0;
  /* add marks */{
    int i;
    struct ledger_table_mark* act_end;
    struct ledger_table_mark* act_mark;
    struct ledger_table const* const table =
      ledger_transaction_get_table_c(act);
    act_mark = ledger_table_begin_c(table);
    act_end = ledger_table_end_c(table);
    if (act_mark != NULL && act_end != NULL){
      for (i = 0; i < commit->pair_count
        && (!ledger_table_mark_is_equal(act_mark, act_end));
        ++i, ledger_table_mark_move(act_mark,+1))
      {
        /* read the row account information */
        unsigned char check_number[64];
        int ledger_index, account_index;
        int ledger_id, account_id;
        ledger_table_fetch_id(act_mark, 0, &ledger_index);
        ledger_table_fetch_id(act_mark, 1, &account_index);
        /* allocate the account row */{
          int ok;
          struct ledger_ledger* ledger =
            ledger_book_get_ledger(book, ledger_index);
          struct ledger_account* account =
            ledger_ledger_get_account(ledger, account_index);
          struct ledger_table* table =
            ledger_account_get_table(account);
          struct ledger_table_mark* next_mark =
            ledger_table_end(table);
          if (next_mark == NULL) break;
          ledger_id = ledger_ledger_get_id(ledger);
          account_id = ledger_account_get_id(account);
          /* add the row */
          ok = ledger_table_add_row(next_mark);
          if (!ok){
            ledger_table_mark_free(next_mark);
            break;
          } else do {
            /* push the information */
            /* journal identifier */{
              ok = ledger_table_put_id(next_mark, 0, journal_id);
              if (!ok) break;
            }
            /* entry identifier */{
              ok = ledger_table_put_id(next_mark, 1, commit->entry_id);
              if (!ok) break;
            }
            /* amount */{
              ok = ledger_table_fetch_bignum(act_mark, 3, tmp_bignum);
              if (!ok) break;
              ok = ledger_table_put_bignum(next_mark, 2, tmp_bignum);
              if (!ok) break;
            }
            /* check number */{
              unsigned char check_number[64];
              ok = ledger_table_fetch_string
                (act_mark, 4, check_number, sizeof(check_number));
              if (ok < 0 || ok >= sizeof(check_number)) break;
              ok = ledger_table_put_string(next_mark, 3, check_number);
              if (!ok) break;
            }
            /* date */{
              ok = ledger_table_put_string(next_mark, 4, date);
              if (!ok) break;
            }
          } while (0);
          if (!ok){
            ledger_table_drop_row(next_mark);
            ledger_table_mark_free(next_mark);
          } else {
            /* persist the mark */
            commit->pairs[i].ledger = next_mark;
          }
        }
        /* allocate the journal row */{
          int ok;
          struct ledger_table_mark* next_mark =
            ledger_table_end(active_j_table);
          if (next_mark == NULL) break;
          /* add the row */
          ok = ledger_table_add_row(next_mark);
          if (!ok){
            ledger_table_mark_free(next_mark);
            break;
          } else do {
            /* push the information */
            /* entry identifier */{
              ok = ledger_table_put_id(next_mark, 0, commit->entry_id);
              if (!ok) break;
            }
            /* ledger identifier */{
              ok = ledger_table_put_id(next_mark, 1, ledger_id);
              if (!ok) break;
            }
            /* account identifier */{
              ok = ledger_table_put_id(next_mark, 2, account_id);
              if (!ok) break;
            }
            /* amount */{
              /*
              ok = ledger_table_fetch_bignum(act_mark, 3, tmp_bignum);
              if (!ok) break;
              */
              ok = ledger_table_put_bignum(next_mark, 3, tmp_bignum);
              if (!ok) break;
            }
            /* check number */{
              /*
              ok = ledger_table_fetch_string
                (act_mark, 4, check_number, sizeof(check_number));
              if (ok < 0 || ok >= sizeof(check_number)) break;
              */
              ok = ledger_table_put_string(next_mark, 4, check_number);
              if (!ok) break;
            }
          } while (0);
          if (!ok){
            ledger_table_drop_row(next_mark);
            ledger_table_mark_free(next_mark);
          } else {
            /* persist the mark */
            commit->pairs[i].journal = next_mark;
          }
        }
      }
      result = (i == commit->pair_count);
    } else result = 0;
    ledger_table_mark_free(act_mark);
    ledger_table_mark_free(act_end);
  }
  ledger_bignum_free(tmp_bignum);
  return result;
}

int ledger_commit_allocate_pairs
  ( struct ledger_commit* commit, struct ledger_transaction const* act)
{
  int const target_count =
    ledger_table_count_rows(
      ledger_transaction_get_table_c(act));
  /* allocate the pairs array */if (target_count > 0) {
    struct ledger_commit_pair *new_pairs;
    ledger_commit_clear(commit);
    new_pairs = ledger_util_malloc(sizeof(struct ledger_commit_pair)*
        target_count);
    if (new_pairs == NULL)
      return 0;
    else {
      int i;
      for (i = 0; i < target_count; ++i){
        new_pairs[i].ledger = NULL;
        new_pairs[i].journal = NULL;
      }
      commit->pairs = new_pairs;
      commit->pair_count = target_count;
    }
  } else {
    ledger_commit_clear(commit);
  }
  return 1;
}

void ledger_commit_clear(struct ledger_commit* commit){
  int i;
  for (i = 0; i < commit->pair_count; ++i){
    ledger_table_mark_free(commit->pairs[i].ledger);
    ledger_table_mark_free(commit->pairs[i].journal);
  }
  ledger_util_free(commit->pairs);
  commit->pairs = NULL;
  commit->pair_count = 0;
  return;
}

void ledger_commit_rollback
  ( struct ledger_commit* commit, struct ledger_book* book,
    struct ledger_transaction const* act)
{
  int i;
  int result;
  int const journal_index = ledger_transaction_get_journal(act);
  struct ledger_journal* const active_journal =
    ledger_book_get_journal(book, journal_index);
  for (i = 0; i < commit->pair_count; ++i){
    if (commit->pairs[i].ledger != NULL)
      ledger_table_drop_row(commit->pairs[i].ledger);
    if (commit->pairs[i].journal != NULL)
      ledger_table_drop_row(commit->pairs[i].journal);
  }
  result = ledger_journal_set_entry_count
    (active_journal, commit->entry_index);
  if (!result){
    struct ledger_entry* const entry =
      ledger_journal_get_entry(active_journal, commit->entry_index);
    ledger_entry_set_id(entry, -1);
  }
  return;
}

/* END   static implementation */

/* BEGIN static implementation */

int ledger_commit_transaction
  (struct ledger_book* book, struct ledger_transaction* act)
{
  int result;
  struct ledger_commit commit;
  /* first pass: resolve account names */{
    result = ledger_commit_verify(book, act);
    if (!result) return 0;
  }
  ledger_commit_init(&commit);
  /* second pass: confirm journal entry */{
    result = ledger_commit_acquire_entry(&commit, book, act);
    if (!result) return 0;
  }
  /* third pass: allocate lines across accounts and journal tables */do {
    result = ledger_commit_acquire_lines(&commit, book, act);
    if (!result) break;
  } while (0);
  if (!result){
    ledger_commit_rollback(&commit, book, act);
  }
  ledger_commit_clear(&commit);
  return result;
}

/* END   implementation */
