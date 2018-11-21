
#include "manage.h"
#include "../base/book.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/table.h"
#include "../base/bignum.h"
#include "../base/find.h"
#include "../act/arg.h"
#include "../act/transact.h"
#include "../act/commit.h"
#include <stddef.h>
#include "../../deps/linenoise/linenoise.h"
#include "line.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int ledger_cli_manage_entry
  ( struct ledger_table_mark* const table_mark, char const* account_path,
    struct ledger_bignum const* next_amount, char const* check_value);

/* BEGIN static implementation */


int ledger_cli_manage_entry
  ( struct ledger_table_mark* const mark, char const* path,
    struct ledger_bignum const* amount, char const* check_value)
{
  int ok = 0;
  do {
    ok = ledger_table_add_row(mark);
    if (!ok) break;
    /* set target account */
    ok = ledger_table_put_string(mark, 2, (unsigned char const*)path);
    if (!ok) break;
    /* set amount */
    ok = ledger_table_put_bignum(mark, 3, amount);
    if (!ok) break;
    /* set check */
    ok = ledger_table_put_string(mark, 4, (unsigned char const*)check_value);
    if (!ok) break;
    ledger_table_mark_move(mark, +1);
  } while (0);
  return ok;
}


/* END   static implementation */


/* BEGIN implementation */

int ledger_cli_make_ledger
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* id_string = NULL;
  int id_note = -1;
  int ledger_index;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-i") == 0){
      if (++argi < argc){
        id_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_ledger: Make a ledger.\n"
      "usage: make_ledger [options] (name)\n"
      "options:\n"
      "  (name)           set name for new ledger\n"
      "  -d (string)      set description for new ledger\n"
      "  -i (number)      id number for new ledger\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* sanity check on names and identifiers */{
    int corresponding_index;
    corresponding_index =
      ledger_find_ledger_by_name(tracking->book, name_string);
    if (corresponding_index >= 0){
      fputs("make_ledger: Ledger by name already exists\n", stderr);
      return 1;
    }
    if (id_string != NULL){
      id_note = atoi(id_string);
      if (id_note < 0){
        fputs("make_ledger: Negative ID not allowed\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_ledger_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_ledger: Ledger by ID already exists\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_journal_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_ledger: Journal by ID already exists\n", stderr);
        return 1;
      }
    }
  }
  /* make the ledger */{
    int ok;
    ledger_index = ledger_book_get_ledger_count(tracking->book);
    ok = ledger_book_set_ledger_count(tracking->book, ledger_index+1);
    if (!ok){
      fputs("make_ledger: Error when making ledger structure\n", stderr);
      return 1;
    } else {
      int soft_ok;
      struct ledger_ledger* ledger =
        ledger_book_get_ledger(tracking->book, ledger_index);
      if (id_string != NULL){
        ledger_ledger_set_id(ledger, id_note);
      }
      soft_ok = ledger_ledger_set_name(ledger, name_string);
      if (!soft_ok){
        fputs("make_ledger: warning: Unable to set name\n", stderr);
      }
      soft_ok = ledger_ledger_set_description(ledger, description_string);
      if (!soft_ok){
        fputs("make_ledger: warning: Unable to set description\n", stderr);
      }
    }
  }
  fprintf(stderr, "make_ledger: Created ledger@%i\n", ledger_index);
  return 0;
}


int ledger_cli_make_journal
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* id_string = NULL;
  int id_note = -1;
  int journal_index;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-i") == 0){
      if (++argi < argc){
        id_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_journal: Make a journal.\n"
      "usage: make_journal [options] (name)\n"
      "options:\n"
      "  (name)           set name for new journal\n"
      "  -d (string)      set description for new journal\n"
      "  -i (number)      id number for new journal\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* sanity check on names and identifiers */{
    int corresponding_index;
    corresponding_index =
      ledger_find_journal_by_name(tracking->book, name_string);
    if (corresponding_index >= 0){
      fputs("make_journal: Journal by name already exists\n", stderr);
      return 1;
    }
    if (id_string != NULL){
      id_note = atoi(id_string);
      if (id_note < 0){
        fputs("make_journal: Negative ID not allowed\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_journal_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_journal: Journal by ID already exists\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_ledger_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_journal: Ledger by ID already exists\n", stderr);
        return 1;
      }
    }
  }
  /* make the journal */{
    int ok;
    journal_index = ledger_book_get_journal_count(tracking->book);
    ok = ledger_book_set_journal_count(tracking->book, journal_index+1);
    if (!ok){
      fputs("make_journal: Error when making journal structure\n", stderr);
      return 1;
    } else {
      int soft_ok;
      struct ledger_journal* journal =
        ledger_book_get_journal(tracking->book, journal_index);
      if (id_string != NULL){
        ledger_journal_set_id(journal, id_note);
      }
      soft_ok = ledger_journal_set_name(journal, name_string);
      if (!soft_ok){
        fputs("make_journal: warning: Unable to set name\n", stderr);
      }
      soft_ok = ledger_journal_set_description(journal, description_string);
      if (!soft_ok){
        fputs("make_journal: warning: Unable to set description\n", stderr);
      }
    }
  }
  fprintf(stderr, "make_journal: Created journal@%i\n", journal_index);
  return 0;
}



int ledger_cli_make_account
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* id_string = NULL;
  char const* pick_ledger_string = NULL;
  int id_note = -1;
  int account_index;
  int ledger_index;
  struct ledger_ledger *pick_ledger;
  if (tracking->object_path.len >= 1
  &&  tracking->object_path.typ == LEDGER_ACT_PATH_LEDGER)
  {
    ledger_index = tracking->object_path.path[0];
  } else ledger_index = -1;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-i") == 0){
      if (++argi < argc){
        id_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-l") == 0){
      if (++argi < argc){
        pick_ledger_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_account: Make an account.\n"
      "usage: make_account [options] (name)\n"
      "options:\n"
      "  (name)           set name for new account\n"
      "  -d (string)      set description for new account\n"
      "  -i (number)      id number for new account\n"
      "  -l (path)        path to desired container ledger\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* resolve ledger path */if (pick_ledger_string != NULL){
    int ok;
    struct ledger_act_path new_path =
      ledger_act_path_compute(tracking->book, pick_ledger_string,
      tracking->object_path, &ok);
    if (!ok){
      fputs("make_account: Error processing path to ledger\n", stderr);
      return 1;
    } else if (new_path.typ != LEDGER_ACT_PATH_LEDGER){
      fputs("make_account: Path does not point to a ledger\n", stderr);
      return 1;
    } else {
      ledger_index = new_path.path[0];
    }
  }
  if (ledger_index < 0){
    fputs("make_account: Choose a ledger before making an account\n", stderr);
    return 1;
  }
  pick_ledger = ledger_book_get_ledger(tracking->book, ledger_index);
  if (pick_ledger == NULL){
    fputs("make_account: Chosen ledger is unavailable\n", stderr);
    return 1;
  }
  /* sanity check on names and identifiers */{
    int corresponding_index;
    corresponding_index =
      ledger_find_account_by_name(pick_ledger, name_string);
    if (corresponding_index >= 0){
      fputs("make_account: Journal by name already exists\n", stderr);
      return 1;
    }
    if (id_string != NULL){
      id_note = atoi(id_string);
      if (id_note < 0){
        fputs("make_account: Negative ID not allowed\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_account_by_id(pick_ledger, id_note);
      if (corresponding_index >= 0){
        fputs("make_account: Journal by ID already exists\n", stderr);
        return 1;
      }
    }
  }
  /* make the account */{
    int ok;
    account_index = ledger_ledger_get_account_count(pick_ledger);
    ok = ledger_ledger_set_account_count(pick_ledger, account_index+1);
    if (!ok){
      fputs("make_account: Error when making account structure\n", stderr);
      return 1;
    } else {
      int soft_ok;
      struct ledger_account* account =
        ledger_ledger_get_account(pick_ledger, account_index);
      if (id_string != NULL){
        ledger_account_set_id(account, id_note);
      }
      soft_ok = ledger_account_set_name(account, name_string);
      if (!soft_ok){
        fputs("make_account: warning: Unable to set name\n", stderr);
      }
      soft_ok = ledger_account_set_description(account, description_string);
      if (!soft_ok){
        fputs("make_account: warning: Unable to set description\n", stderr);
      }
    }
  }
  fprintf(stderr, "make_account: Created account@%i\n", account_index);
  return 0;
}



int ledger_cli_make_entry
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int result;
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* time_string = NULL;
  char const* pick_journal_string = NULL;
  int id_note = -1;
  int account_index;
  int journal_index;
  struct ledger_journal *pick_journal;
  if (tracking->object_path.len >= 1
  &&  tracking->object_path.typ == LEDGER_ACT_PATH_JOURNAL)
  {
    journal_index = tracking->object_path.path[0];
  } else journal_index = -1;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-t") == 0){
      if (++argi < argc){
        time_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-j") == 0){
      if (++argi < argc){
        pick_journal_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_entry: Make a journal entry.\n"
      "usage: make_entry [options] (name)\n"
      "options:\n"
      "  (name)           set name for new entry\n"
      "  -d (string)      set description for new entry\n"
      "  -t (date)        set date and time for new entry\n"
      "  -j (path)        path to desired container journal\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* resolve journal path */if (pick_journal_string != NULL){
    int ok;
    struct ledger_act_path new_path =
      ledger_act_path_compute(tracking->book, pick_journal_string,
      tracking->object_path, &ok);
    if (!ok){
      fputs("make_entry: Error processing path to journal\n", stderr);
      return 1;
    } else if (new_path.typ != LEDGER_ACT_PATH_JOURNAL){
      fputs("make_entry: Path does not point to a journal\n", stderr);
      return 1;
    } else {
      journal_index = new_path.path[0];
    }
  }
  if (journal_index < 0){
    fputs("make_entry: Choose a journal before making an account\n", stderr);
    return 1;
  }
  pick_journal = ledger_book_get_journal(tracking->book, journal_index);
  if (pick_journal == NULL){
    fputs("make_entry: Chosen journal is unavailable\n", stderr);
    return 1;
  }
  /* prepare the transaction */{
    int ok;
    struct ledger_bignum* next_amount;
    struct ledger_arg_list* list_pull;
    struct ledger_transaction* next_transaction;
    struct ledger_table_mark* table_mark = NULL;
    list_pull = ledger_arg_list_new();
    if (list_pull == NULL){
      fputs("make_entry: Error when making argument capture"
        " structure\n", stderr);
      return 0;
    }
    next_amount = ledger_bignum_new();
    if (next_amount == NULL){
      fputs("make_entry: Error when making numeric capture"
        " structure\n", stderr);
      ledger_arg_list_free(list_pull);
      return 0;
    }
    next_transaction = ledger_transaction_new();
    if (next_transaction == NULL){
      fputs("make_entry: Error when making transaction"
        " structure\n", stderr);
      ok = 0;
    } else do {
      int done = 0;
      ok = 0;
      ledger_transaction_set_journal(next_transaction, journal_index);
      ok = ledger_transaction_set_date(next_transaction, time_string);
      if (!ok) {
        fputs("make_entry: Error when setting transaction date\n",
            stderr);
        break;
      }
      ok = ledger_transaction_set_name(next_transaction, name_string);
      if (!ok) {
        fputs("make_entry: Error when setting transaction name\n",
            stderr);
        break;
      }
      /* acquire mark */{
        struct ledger_table* const table =
          ledger_transaction_get_table(next_transaction);
        table_mark = ledger_table_end(table);
        if (table_mark == NULL){
          fputs("make_entry: Error when accessing transaction"
            " structure\n", stderr);
          break;
        }
      }
      /* transaction line loop */
      while (!done){
        char* next_line = ledger_cli_get_sub_line(tracking, "> ");
        if (next_line == NULL) {
          done = 1;
        } else if (*next_line == 0) {
          ledger_cli_free_line(tracking, next_line);
          done = 1;
        } else {
          char const* first;
          ok = ledger_arg_list_parse(list_pull, next_line);
          ledger_cli_free_line(tracking, next_line);
          if (!ok) {
            fputs("make_entry: Error when parsing "
              "transaction line\n", stderr);
            break;
          }
          first = ledger_arg_list_get(list_pull, 0);
          if (first == NULL) break;
          else if (strcmp(first,"cancel") == 0){
            /* signal request to cancel by setting */done = -1;
          } else if (strcmp(first,"debit") == 0){
            char const* account_path;
            char const* check_value = NULL;
            int const arg_count = ledger_arg_list_get_count(list_pull);
            if (arg_count >= 3){
              /* get amount */
              ok = ledger_bignum_set_text
                ( next_amount, ledger_arg_list_get(list_pull, 1), NULL);
              if (!ok) {
                fputs("make_entry: Error when parsing "
                  "line amount\n", stderr);
                break;
              }
              /* get account path */
              account_path = ledger_arg_list_get(list_pull, 2);
              /* get check value */if (arg_count > 3){
                check_value = ledger_arg_list_get(list_pull, 3);
              }
            } else break;
            /* compose the entry */
            ok = ledger_cli_manage_entry
              (table_mark, account_path, next_amount, check_value);
            if (!ok) {
              fputs("make_entry: Error encountered when crafting "
                "transaction line\n", stderr);
              break;
            }
          } else if (strcmp(first,"credit") == 0){
            char const* account_path;
            char const* check_value = NULL;
            int const arg_count = ledger_arg_list_get_count(list_pull);
            if (arg_count >= 3){
              /* get amount */
              ok = ledger_bignum_set_text
                ( next_amount, ledger_arg_list_get(list_pull, 1), NULL);
              if (!ok) {
                fputs("make_entry: Error when parsing "
                  "line amount\n", stderr);
                break;
              }
              ok = ledger_bignum_negate(next_amount, next_amount);
              if (!ok) {
                fputs("make_entry: Error when preparing "
                  "line amount as a credit\n", stderr);
                break;
              }
              /* get account path */
              account_path = ledger_arg_list_get(list_pull, 2);
              /* get check value */if (arg_count > 3){
                check_value = ledger_arg_list_get(list_pull, 3);
              }
            } else break;
            /* compose the entry */
            ok = ledger_cli_manage_entry
              (table_mark, account_path, next_amount, check_value);
            if (!ok) {
              fputs("make_entry: Error encountered when crafting "
                "transaction line\n", stderr);
              break;
            }
          }
        }
      }
      if (ok){
        if (done == 1){
          int balance;
          /* check balance of transaction */
          ok = ledger_commit_check_balance(next_transaction, &balance);
          if (!ok) break;
          if (!balance){
            fputs("make_entry: Unbalanced transaction rejected\n", stderr);
            ok = 0;
            break;
          }
          /* submit transaction */
          ok = ledger_commit_transaction(tracking->book, next_transaction);
        } else {
          ok = -1;
        }
      } else break;
    } while (0);
    ledger_table_mark_free(table_mark);
    ledger_transaction_free(next_transaction);
    ledger_arg_list_free(list_pull);
    ledger_bignum_free(next_amount);
    if (ok == -1){
      fputs("make_entry: Transaction cancelled by user\n", stderr);
      result = 0;
    } else if (!ok) {
      fputs("make_entry: Error when executing transaction\n", stderr);
      result = 1;
    } else {
      fputs("make_entry: Transaction execution complete\n", stderr);
      result = 0;
    }
  }
  return result;
}


/* END   implementation */
