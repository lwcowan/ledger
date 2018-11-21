
#include "rename.h"
#include "../base/book.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "line.h"
#include <stdio.h>


/* BEGIN static implementation */


/* END   static implementation */


/* BEGIN implementation */



int ledger_cli_rename
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int result;
  struct ledger_act_path new_path;
  struct ledger_book* book = tracking->book;
  if (argc < 3){
    fprintf(stderr,"usage: rename (path_to_object) (new_name)\n");
    return 2;
  }
  new_path = ledger_act_path_compute
    (book, argv[1], tracking->object_path, &result);
  if (result == 0){
    fprintf(stderr,"rename: Error encountered in processing path\n");
    return 2;
  }
  switch (new_path.typ){
  case LEDGER_ACT_PATH_BOOK:
    {
      fprintf(stderr,"Book has no name.\n");
      result = 0;
    }break;
  case LEDGER_ACT_PATH_LEDGER:
    {
      struct ledger_ledger* ledger =
          ledger_book_get_ledger(book, new_path.path[0]);
      if (ledger == NULL){
        fprintf(stderr,"Ledger unavailable.\n");
        result = 0;
      } else {
        result = ledger_ledger_set_name(ledger, argv[2]);
      }
    }break;
  case LEDGER_ACT_PATH_JOURNAL:
    {
      struct ledger_journal* journal =
          ledger_book_get_journal(book, new_path.path[0]);
      if (journal == NULL){
        fprintf(stderr,"Journal unavailable.\n");
        result = 0;
      } else {
        result = ledger_journal_set_name(journal, argv[2]);
      }
    }break;
  case LEDGER_ACT_PATH_ENTRY:
    {
      struct ledger_journal* journal =
          ledger_book_get_journal(book, new_path.path[0]);
      struct ledger_entry* entry;
      if (journal == NULL){
        fprintf(stderr,"Journal unavailable.\n");
        result = 0;
        break;
      }
      entry = ledger_journal_get_entry
        (journal, new_path.path[1]);
      if (entry == NULL){
        fprintf(stderr,"Entry unavailable.\n");
        result = 0;
        break;
      } else {
        result = ledger_entry_set_name(entry, argv[2]);
      }
    }break;
  case LEDGER_ACT_PATH_ACCOUNT:
    {
      struct ledger_ledger* ledger =
          ledger_book_get_ledger(book, new_path.path[0]);
      struct ledger_account* account;
      if (ledger == NULL){
        fprintf(stderr,"Ledger unavailable.\n");
        result = 0;
        break;
      }
      account = ledger_ledger_get_account
        (ledger, new_path.path[1]);
      if (account == NULL){
        fprintf(stderr,"Account unavailable.\n");
        result = 0;
        break;
      } else {
        result = ledger_account_set_name(account, argv[2]);
      }
    }break;
  default:
    result = 0;
    break;
  }
  return result?0:1;
}

/* END   implementation */
