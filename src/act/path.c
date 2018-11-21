
#include "path.h"
#include "../base/util.h"
#include <stddef.h>
#include "../base/book.h"
#include "../base/find.h"
#include "../base/ledger.h"
#include "../base/journal.h"


/* BEGIN static implementation */



/* END   static implementation */

/* BEGIN implementation */

struct ledger_act_path ledger_act_path_root(void){
  struct ledger_act_path out;
  out.path[0] = -1;
  out.path[1] = -1;
  out.len = 0;
  out.typ = 1;
  return out;
}



struct ledger_act_path ledger_act_path_compute
  ( struct ledger_book const* book, unsigned char const* path_text,
    struct ledger_act_path start, int *ok)
{
  int slash_count = 0;
  struct ledger_act_path finish;
  int completion = 1;
  unsigned char const* up = (unsigned char const*)"..";
  unsigned char const* here = (unsigned char const*)".";
  unsigned char* p;
  unsigned char* token_point;
  int strdup_ok;
  int const fixed_length = sizeof(finish.path)/sizeof(finish.path[0]);
  unsigned char* short_temp =
    ledger_util_ustrdup(path_text, &strdup_ok);
  if (short_temp == NULL){
    *ok = 0;
    return start;
  }
  token_point = short_temp;
  if (token_point[0] == '/'){
    finish.path[0] = -1;
    finish.path[1] = -1;
    finish.len = 0;
    finish.typ = 1;
    token_point += 1;
  } else finish = start;
  /* for each path piece */{
    for (p = token_point; completion; ++p){
      int end_of_string = 0;
      if (*p == '/' || *p == 0){
        if (*p == 0) end_of_string = 1;
        else *p = 0;
        /* process the path component */if (token_point[0] != 0){
          if (ledger_util_ustrcmp(here, token_point) == 0){
            /* do nothing */
          } else if (ledger_util_ustrcmp(up, token_point) == 0){
            if (finish.len > 0){
              finish.len -= 1;
              finish.path[finish.len] = -1;
              switch (finish.typ){
              case LEDGER_ACT_PATH_LEDGER:
              case LEDGER_ACT_PATH_JOURNAL:
                finish.typ = LEDGER_ACT_PATH_BOOK;
                break;
              case LEDGER_ACT_PATH_ACCOUNT:
                finish.typ = LEDGER_ACT_PATH_LEDGER;
                break;
              }
            }
          } else switch (finish.typ){
          case LEDGER_ACT_PATH_BOOK:
            {
              /* parse the path */
              int next_typ = 0;
              int next_index = -1;
              if (ledger_util_ustrncmp("ledger:",token_point,7) == 0){
                unsigned char const* next_name = token_point+7;
                next_index = ledger_find_ledger_by_name(book, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_LEDGER;
              } else if (ledger_util_ustrncmp("ledger#",token_point,7) == 0){
                int next_id = ledger_util_atoi(token_point+7);
                next_index = ledger_find_ledger_by_id(book, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_LEDGER;
              } else if (ledger_util_ustrncmp("ledger@",token_point,7) == 0){
                next_index = ledger_util_atoi(token_point+7);
                if (next_index < 0
                ||  next_index >= ledger_book_get_ledger_count(book))
                {
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_LEDGER;
              } else if (ledger_util_ustrncmp("journal:",token_point,8) == 0){
                unsigned char const* next_name = token_point+8;
                next_index = ledger_find_journal_by_name(book, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_JOURNAL;
              } else if (ledger_util_ustrncmp("journal#",token_point,8) == 0){
                int next_id = ledger_util_atoi(token_point+8);
                next_index = ledger_find_journal_by_id(book, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_JOURNAL;
              } else if (ledger_util_ustrncmp("journal@",token_point,8) == 0){
                next_index = ledger_util_atoi(token_point+8);
                if (next_index < 0
                ||  next_index >= ledger_book_get_journal_count(book))
                {
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_JOURNAL;
              } else completion = 0;
              /* post the navigation */
              if (next_typ != 0 && finish.len < fixed_length){
                finish.typ = next_typ;
                finish.path[finish.len] = next_index;
                finish.len += 1;
              }
            }break;
          case LEDGER_ACT_PATH_LEDGER:
            {
              /* parse the path */
              struct ledger_ledger const* ledger =
                ledger_book_get_ledger_c(book, finish.path[0]);
              int next_typ = 0;
              int next_index = -1;
              if (ledger == NULL){
                completion = 0;
                break;
              } else if (ledger_util_ustrncmp("account:",token_point,8) == 0){
                unsigned char const* next_name = token_point+8;
                next_index = ledger_find_account_by_name(ledger, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ACCOUNT;
              } else if (ledger_util_ustrncmp("account#",token_point,8) == 0){
                int next_id = ledger_util_atoi(token_point+8);
                next_index = ledger_find_account_by_id(ledger, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ACCOUNT;
              } else if (ledger_util_ustrncmp("account@",token_point,8) == 0){
                next_index = ledger_util_atoi(token_point+8);
                if (next_index < 0
                ||  next_index >= ledger_ledger_get_account_count(ledger))
                {
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ACCOUNT;
              } else completion = 0;
              /* post the navigation */
              if (next_typ != 0 && finish.len < fixed_length){
                finish.typ = next_typ;
                finish.path[finish.len] = next_index;
                finish.len += 1;
              }
            }break;
          case LEDGER_ACT_PATH_JOURNAL:
            {
              /* parse the path */
              struct ledger_journal const* journal =
                ledger_book_get_journal_c(book, finish.path[0]);
              int next_typ = 0;
              int next_index = -1;
              if (journal == NULL){
                completion = 0;
                break;
              } else if (ledger_util_ustrncmp("entry:",token_point,6) == 0){
                unsigned char const* next_name = token_point+6;
                next_index = ledger_find_entry_by_name(journal, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ENTRY;
              } else if (ledger_util_ustrncmp("entry#",token_point,6) == 0){
                int next_id = ledger_util_atoi(token_point+6);
                next_index = ledger_find_entry_by_id(journal, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ENTRY;
              } else if (ledger_util_ustrncmp("entry@",token_point,6) == 0){
                next_index = ledger_util_atoi(token_point+6);
                if (next_index < 0
                ||  next_index >= ledger_journal_get_entry_count(journal))
                {
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ENTRY;
              } else completion = 0;
              /* post the navigation */
              if (next_typ != 0 && finish.len < fixed_length){
                finish.typ = next_typ;
                finish.path[finish.len] = next_index;
                finish.len += 1;
              }
            }break;
          default:
            completion = 0;
            break;
          }
        }
        token_point = p+1;
      } else continue;
      /* end of string! */if (end_of_string) break;
    }
  }
  ledger_util_free(short_temp);
  /* terminal analysis */
  if (completion){
    *ok = 1;
    return finish;
  } else {
    *ok = 0;
    return start;
  }
}


/* END   implementation */
