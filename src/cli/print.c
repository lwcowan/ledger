
#include "print.h"
#include "../base/book.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/table.h"
#include "../base/find.h"
#include "line.h"
#include "../act/path.h"
#include <stdio.h>
#include <string.h>


/* BEGIN static implementation */


/* END   static implementation */

/* BEGIN implementation */



int ledger_cli_print_account_line
  ( struct ledger_cli_line *tracking,
    struct ledger_table_mark const* mark, FILE* f)
{
  int result;
  unsigned char path_text[60];
  unsigned char amount_text[16];
  unsigned char check_text[16];
  unsigned char date_text[24];
  do {
    int row_journal_id, row_entry_id;
    if (!ledger_table_fetch_id(mark,0,&row_entry_id)){
      result = 0;
      break;
    }
    struct ledger_act_path display_path;
    if (!ledger_table_fetch_id(mark,0,&row_journal_id)){
      result = 0;
      break;
    }
    if (!ledger_table_fetch_id(mark,1,&row_entry_id)){
      result = 0;
      break;
    }
    display_path.path[0] =
      ledger_find_journal_by_id(tracking->book, row_journal_id);
    if (display_path.path[0] >= 0){
      struct ledger_journal const* row_journal =
        ledger_book_get_journal(tracking->book, display_path.path[0]);
      display_path.path[1] =
        ledger_find_entry_by_id(row_journal, row_entry_id);
    } else display_path.path[1] = -1;
    display_path.typ = LEDGER_ACT_PATH_ENTRY;
    display_path.len = 2;
    if (ledger_act_path_render
        ( path_text, sizeof(path_text), display_path, tracking->book)
        < 0)
      break;
    if (ledger_table_fetch_string
        (mark,2,amount_text,sizeof(amount_text)) < 0)
    {
      result = 0;
      break;
    }
    if (ledger_table_fetch_string
        (mark,3,check_text,sizeof(check_text)) < 0)
    {
      result = 0;
      break;
    }
    if (ledger_table_fetch_string
        (mark,4,date_text,sizeof(date_text)) < 0)
    {
      result = 0;
      break;
    }
    fprintf(f,"  %-60s\n    %16s %24s %16s\n",
        path_text, amount_text, date_text, check_text);
    result = 1;
  } while (0);
  return result;
}


/* END   implementation */
