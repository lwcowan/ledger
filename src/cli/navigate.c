
#include "navigate.h"
#include "../base/book.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/table.h"
#include "../base/sum.h"
#include "../base/bignum.h"
#include "../base/find.h"
#include "line.h"
#include "print.h"
#include <stdio.h>




/* BEGIN static implementation */


/* END   static implementation */


/* BEGIN implementation */

int ledger_cli_list(struct ledger_cli_line *tracking, int argc, char **argv){
  int result;
  struct ledger_book const* const book = tracking->book;
  struct ledger_act_path new_path;
  if (argc > 1){
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)argv[1], tracking->object_path, &result);
    if (result == 0){
      fprintf(stderr,"list: Error encountered in processing path\n");
      return 2;
    }
  } else new_path = tracking->object_path;
  switch (new_path.typ){
  case LEDGER_ACT_PATH_BOOK:
    {
      /* read the book */
      int const ledger_count = ledger_book_get_ledger_count(book);
      int const journal_count = ledger_book_get_journal_count(book);
      fprintf(stdout,"total ledgers: %i\n", ledger_count);
      fprintf(stdout,"total journals: %i\n", journal_count);
      /* list ledgers */{
        int i;
        for (i = 0; i < ledger_count; ++i){
          struct ledger_ledger const* const ledger =
            ledger_book_get_ledger_c(book, i);
          unsigned char const* name = ledger_ledger_get_name(ledger);
          int const item_id = ledger_ledger_get_id(ledger);
          if (name != NULL)
            fprintf(stdout,"  ledger:%s/\n", name);
          else if (item_id >= 0)
            fprintf(stdout,"  ledger#%i/\n", item_id);
          else
            fprintf(stdout,"  ledger@%i/\n", i);
        }
      }
      /* list journals */{
        int i;
        for (i = 0; i < journal_count; ++i){
          struct ledger_journal const* const journal =
            ledger_book_get_journal_c(book, i);
          unsigned char const* name = ledger_journal_get_name(journal);
          int const item_id = ledger_journal_get_id(journal);
          if (name != NULL)
            fprintf(stdout,"  journal:%s/\n", name);
          else if (item_id >= 0)
            fprintf(stdout,"  journal#%i/\n", item_id);
          else
            fprintf(stdout,"  journal@%i/\n", i);
        }
      }
      result = 1;
    }break;
  case LEDGER_ACT_PATH_LEDGER:
    {
      struct ledger_ledger const* const ledger =
          ledger_book_get_ledger_c(book, new_path.path[0]);
      if (ledger == NULL){
        fprintf(stderr,"Ledger unavailable.\n");
        result = 0;
      } else {
        int const account_count = ledger_ledger_get_account_count(ledger);
        fprintf(stdout,"total accounts: %i\n", account_count);
        /* list accounts */{
          int i;
          for (i = 0; i < account_count; ++i){
            struct ledger_account const* const account =
              ledger_ledger_get_account_c(ledger, i);
            unsigned char const* name = ledger_account_get_name(account);
            int const item_id = ledger_account_get_id(account);
            if (name != NULL)
              fprintf(stdout,"  account:%s/\n", name);
            else if (item_id >= 0)
              fprintf(stdout,"  account#%i/\n", item_id);
            else
              fprintf(stdout,"  account@%i/\n", i);
          }
        }
        result = 1;
      }
    }break;
  case LEDGER_ACT_PATH_JOURNAL:
    {
      struct ledger_journal const* const journal =
          ledger_book_get_journal_c(book, new_path.path[0]);
      if (journal == NULL){
        fprintf(stderr,"Journal unavailable.\n");
        result = 0;
      } else {
        int const entry_count = ledger_journal_get_entry_count(journal);
        fprintf(stdout,"total entries: %i\n", entry_count);
        /* list entrys */{
          int i;
          for (i = 0; i < entry_count; ++i){
            struct ledger_entry const* const entry =
              ledger_journal_get_entry_c(journal, i);
            unsigned char const* name = ledger_entry_get_name(entry);
            int const item_id = ledger_entry_get_id(entry);
            if (name != NULL)
              fprintf(stdout,"  entry:%s/\n", name);
            else if (item_id >= 0)
              fprintf(stdout,"  entry#%i/\n", item_id);
            else
              fprintf(stdout,"  entry@%i/\n", i);
          }
        }
        result = 1;
      }
    }break;
  case LEDGER_ACT_PATH_ENTRY:
    {
      struct ledger_journal const* const journal =
          ledger_book_get_journal_c(book, new_path.path[0]);
      struct ledger_entry const* entry;
      if (journal == NULL){
        fprintf(stderr,"Journal unavailable.\n");
        result = 0;
        break;
      }
      entry = ledger_journal_get_entry_c
        (journal, new_path.path[1]);
      if (entry == NULL){
        fprintf(stderr,"Entry unavailable.\n");
        result = 0;
        break;
      }
      /* list transaction lines */{
        int const item_id_entry = ledger_entry_get_id(entry);
        struct ledger_table_mark *mark, *end;
        unsigned char path_text[60];
        unsigned char amount_text[16];
        unsigned char check_text[16];
        int line_count = 0;
        /* get table */{
          struct ledger_table const *table =
            ledger_journal_get_table_c(journal);
          mark = ledger_table_begin_c(table);
          end = ledger_table_end_c(table);
        }
        if (mark != NULL && end != NULL){
          result = 1;
          while (!ledger_table_mark_is_equal(mark,end)){
            int row_entry_id;
            int row_ledger_id, row_account_id;
            if (!ledger_table_fetch_id(mark,0,&row_entry_id)){
              result = 0;
              break;
            }
            if (row_entry_id == item_id_entry){
              struct ledger_act_path display_path;
              line_count += 1;
              if (!ledger_table_fetch_id(mark,1,&row_ledger_id)){
                result = 0;
                break;
              }
              if (!ledger_table_fetch_id(mark,2,&row_account_id)){
                result = 0;
                break;
              }
              display_path.path[0] =
                ledger_find_ledger_by_id(tracking->book, row_ledger_id);
              if (display_path.path[0] >= 0){
                struct ledger_ledger const* row_ledger =
                  ledger_book_get_ledger
                      (tracking->book, display_path.path[0]);
                display_path.path[1] =
                  ledger_find_account_by_id(row_ledger, row_account_id);
              } else display_path.path[1] = -1;
              display_path.typ = LEDGER_ACT_PATH_ACCOUNT;
              display_path.len = 2;
              if (ledger_act_path_render
                  ( path_text, sizeof(path_text), display_path, tracking->book)
                  < 0)
                break;
              if (ledger_table_fetch_string
                  (mark,3,amount_text,sizeof(amount_text)) < 0)
              {
                result = 0;
                break;
              }
              if (ledger_table_fetch_string
                  (mark,4,check_text,sizeof(check_text)) < 0)
              {
                result = 0;
                break;
              }
              fprintf(stderr,"  %-60s\n    %16s %16s\n",
                  path_text, amount_text, check_text);
            }
            ledger_table_mark_move(mark, +1);
          }
          if (result == 0){
            fprintf(stderr,"list: Transaction lines broken\n");
          } else {
            fprintf(stderr,"transaction lines: %i\n", line_count);
          }
        } else {
          fprintf(stderr,"list: Transaction lines unavailable\n");
          result = 0;
          break;
        }
        ledger_table_mark_free(mark);
        ledger_table_mark_free(end);
      }
    }break;
  case LEDGER_ACT_PATH_ACCOUNT:
    {
      struct ledger_ledger const* const ledger =
          ledger_book_get_ledger_c(book, new_path.path[0]);
      struct ledger_account const* account;
      if (ledger == NULL){
        fprintf(stderr,"Ledger unavailable.\n");
        result = 0;
        break;
      }
      account = ledger_ledger_get_account_c
        (ledger, new_path.path[1]);
      if (account == NULL){
        fprintf(stderr,"Account unavailable.\n");
        result = 0;
        break;
      }
      /* report the current account balance */{
        int sum_ok = 0;
        unsigned char sum_buffer[64];
        struct ledger_bignum *sum;
        struct ledger_table const* account_table =
          ledger_account_get_table_c(account);
        sum = ledger_bignum_new();
        if (sum != NULL){
          sum_ok = ledger_sum_table_column(sum, account_table, 2);
        }
        if (sum_ok){
          sum_ok = ledger_bignum_get_text
            (sum, sum_buffer, sizeof(sum_buffer), 1);
          sum_ok = (sum_ok >= 0 && sum_ok < sizeof(sum_buffer));
        }
        ledger_bignum_free(sum);
        if (sum_ok)
          fprintf(stdout, "balance: %s\n", sum_buffer);
        else
          fprintf(stdout, "balance unavailable\n");
      }
      fprintf(stdout, "transaction lines: %i\n",
          ledger_table_count_rows(ledger_account_get_table_c(account)));
      /* list transaction lines */{
        struct ledger_table_mark *mark, *end;
        int line_count = 0;
        /* get table */{
          struct ledger_table const *table =
            ledger_account_get_table_c(account);
          mark = ledger_table_begin_c(table);
          end = ledger_table_end_c(table);
        }
        if (mark != NULL && end != NULL){
          result = 1;
          while (!ledger_table_mark_is_equal(mark,end)){
            ledger_cli_print_account_line(tracking, mark, stderr);
            line_count += 1;
            ledger_table_mark_move(mark, +1);
          }
          if (result == 0){
            fprintf(stderr,"list: Transaction lines broken\n");
          }
        } else {
          fprintf(stderr,"list: Transaction lines unavailable\n");
          result = 0;
          break;
        }
        ledger_table_mark_free(mark);
        ledger_table_mark_free(end);
      }
    }break;
  default:
    result = 0;
    break;
  }
  return result?0:1;
}


int ledger_cli_info(struct ledger_cli_line *tracking, int argc, char **argv){
  int result;
  struct ledger_book const* const book = tracking->book;
  struct ledger_act_path new_path;
  if (argc > 1){
    new_path = ledger_act_path_compute
      (book, (unsigned char const*)argv[1], tracking->object_path, &result);
    if (result == 0){
      fprintf(stderr,"info: Error encountered in processing path\n");
      return 2;
    }
  } else new_path = tracking->object_path;
  switch (new_path.typ){
  case LEDGER_ACT_PATH_BOOK:
    {
      /* read the book */
      int const ledger_count = ledger_book_get_ledger_count(book);
      int const journal_count = ledger_book_get_journal_count(book);
      unsigned char const* note_string = ledger_book_get_notes(book);
      unsigned char const* description_string =
        ledger_book_get_description(book);
      fprintf(stdout,"total ledgers: %i\n", ledger_count);
      fprintf(stdout,"total journals: %i\n", journal_count);
      if (description_string != NULL && description_string[0] != 0){
        fprintf(stdout,"description {\n%s\n}\n",
          (char const*)description_string);
      }
      if (note_string != NULL && note_string[0] != 0){
        fprintf(stdout,"notes {\n%s\n}\n", (char const*)note_string);
      }
      result = 1;
    }break;
  case LEDGER_ACT_PATH_LEDGER:
    {
      struct ledger_ledger const* const ledger =
          ledger_book_get_ledger_c(book, new_path.path[0]);
      if (ledger == NULL){
        fprintf(stderr,"Ledger unavailable.\n");
        result = 0;
      } else {
        unsigned char const* name_string = ledger_ledger_get_name(ledger);
        unsigned char const* description_string =
          ledger_ledger_get_description(ledger);
        int const account_count = ledger_ledger_get_account_count(ledger);
        int const item_id = ledger_ledger_get_id(ledger);
        fprintf(stdout,"total accounts: %i\n", account_count);
        fprintf(stdout,"index path: /ledger@%i\n",
            new_path.path[0]
          );
        if (item_id >= 0){
          fprintf(stdout,"id: %i\n", item_id);
        }
        if (name_string != NULL && name_string[0] != 0){
          fprintf(stdout,"name: %s\n", (char const*)name_string);
        }
        if (description_string != NULL && description_string[0] != 0){
          fprintf(stdout,"description {\n%s\n}\n",
            (char const*)description_string);
        }
        result = 1;
      }
    }break;
  case LEDGER_ACT_PATH_JOURNAL:
    {
      struct ledger_journal const* const journal =
          ledger_book_get_journal_c(book, new_path.path[0]);
      if (journal == NULL){
        fprintf(stderr,"Journal unavailable.\n");
        result = 0;
      } else {
        unsigned char const* name_string = ledger_journal_get_name(journal);
        unsigned char const* description_string =
          ledger_journal_get_description(journal);
        int const entry_count = ledger_journal_get_entry_count(journal);
        int const item_id = ledger_journal_get_id(journal);
        fprintf(stdout,"total entries: %i\n", entry_count);
        fprintf(stdout,"index path: /journal@%i\n",
            new_path.path[0]
          );
        if (item_id >= 0){
          fprintf(stdout,"id: %i\n", item_id);
        }
        if (name_string != NULL && name_string[0] != 0){
          fprintf(stdout,"name: %s\n", (char const*)name_string);
        }
        if (description_string != NULL && description_string[0] != 0){
          fprintf(stdout,"description {\n%s\n}\n",
            (char const*)description_string);
        }
        result = 1;
      }
    }break;
  case LEDGER_ACT_PATH_ENTRY:
    {
      struct ledger_journal const* const journal =
          ledger_book_get_journal_c(book, new_path.path[0]);
      struct ledger_entry const* entry;
      if (journal == NULL){
        fprintf(stderr,"Journal unavailable.\n");
        result = 0;
        break;
      }
      entry = ledger_journal_get_entry_c
        (journal, new_path.path[1]);
      if (entry == NULL){
        fprintf(stderr,"Entry unavailable.\n");
        result = 0;
        break;
      }
      /* print information */{
        unsigned char const* name = ledger_entry_get_name(entry);
        unsigned char const* date = ledger_entry_get_date(entry);
        unsigned char const* description =
          ledger_entry_get_description(entry);
        int const item_id = ledger_entry_get_id(entry);
        fprintf(stdout,"index path: /journal@%i/entry@%i\n",
            new_path.path[0],
            new_path.path[1]
          );
        if (item_id >= 0){
          fprintf(stdout,"id: %i\n", item_id);
        }
        if (name != NULL && name[0] != 0){
          fprintf(stdout,"name: %s\n", (char const*)name);
        }
        if (date != NULL && date[0] != 0){
          fprintf(stdout,"date: %s\n", (char const*)date);
        }
        if (description != NULL && description[0] != 0){
          fprintf(stdout,"description {\n%s\n}\n",
            (char const*)description);
        }
      }
      result = 1;
    }break;
  case LEDGER_ACT_PATH_ACCOUNT:
    {
      struct ledger_ledger const* const ledger =
          ledger_book_get_ledger_c(book, new_path.path[0]);
      struct ledger_account const* account;
      if (ledger == NULL){
        fprintf(stderr,"Ledger unavailable.\n");
        result = 0;
        break;
      }
      account = ledger_ledger_get_account_c
        (ledger, new_path.path[1]);
      if (account == NULL){
        fprintf(stderr,"Account unavailable.\n");
        result = 0;
        break;
      }
      /* report the current account balance */{
        int sum_ok = 0;
        unsigned char sum_buffer[64];
        struct ledger_bignum *sum;
        struct ledger_table const* account_table =
          ledger_account_get_table_c(account);
        sum = ledger_bignum_new();
        if (sum != NULL){
          sum_ok = ledger_sum_table_column(sum, account_table, 2);
        }
        if (sum_ok){
          sum_ok = ledger_bignum_get_text
            (sum, sum_buffer, sizeof(sum_buffer), 1);
          sum_ok = (sum_ok >= 0 && sum_ok < sizeof(sum_buffer));
        }
        ledger_bignum_free(sum);
        if (sum_ok)
          fprintf(stdout, "balance: %s\n", sum_buffer);
        else
          fprintf(stdout, "balance unavailable\n");
      }
      fprintf(stdout, "transaction lines: %i\n",
          ledger_table_count_rows(ledger_account_get_table_c(account)));
      /* print information */{
        unsigned char const* name = ledger_account_get_name(account);
        unsigned char const* description =
          ledger_account_get_description(account);
        int const item_id = ledger_account_get_id(account);
        fprintf(stdout,"index path: /ledger@%i/account@%i\n",
            new_path.path[0],
            new_path.path[1]
          );
        if (item_id >= 0){
          fprintf(stdout,"id: %i\n", item_id);
        }
        if (name != NULL && name[0] != 0){
          fprintf(stdout,"name: %s\n", (char const*)name);
        }
        if (description != NULL && description[0] != 0){
          fprintf(stdout,"description {\n%s\n}\n",
            (char const*)description);
        }
      }
      result = 1;
    }break;
  default:
    result = 0;
    break;
  }
  return result?0:1;
}

int ledger_cli_enter(struct ledger_cli_line *tracking, int argc, char **argv){
  int result;
  struct ledger_book const* const book = tracking->book;
  if (argc < 2){
    fputs("enter: Enter an object.\n"
      "usage: enter (path_to_object)\n",stderr);
    return 2;
  }
  /* compute new path */{
    int ok;
    struct ledger_act_path const new_path =
      ledger_act_path_compute(book, (unsigned char const*)argv[1],
                  tracking->object_path, &ok);
    if (!ok){
      fputs("enter: Object not found.\n",stderr);
      result = 1;
    } else {
      tracking->object_path = new_path;
      result = 0;
    }
  }
  return result;
}

/* END   implementation */
