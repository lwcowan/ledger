
#include "path.h"
#include "../base/util.h"
#include <stddef.h>
#include "../base/book.h"
#include "../base/find.h"
#include "../base/ledger.h"
#include "../base/journal.h"
#include "../base/account.h"
#include "../base/entry.h"
#include <limits.h>

/*
 * Push a string into a buffer.
 * - buf buffer targeted
 * - len length of buffer
 * - string string to push
 * @return the length of the string
 */
int ledger_act_path_push_string
  (unsigned char* buf, int len, char const* string);

/*
 * Push an integer into a buffer.
 * - buf buffer targeted
 * - len length of buffer
 * - n integer to push
 * @return the length of the string representing the integer
 */
int ledger_act_path_push_int
  (unsigned char* buf, int len, int n);

/*
 * Push an object token into a buffer.
 * - buf buffer targeted
 * - len length of buffer
 * - type type of object
 * - name name of object
 * - item_id identifier of object
 * - i object index
 * @return the length of the token
 */
int ledger_act_path_push_token
  ( unsigned char* buf, int len, int typ,
    unsigned char const* name, int item_id, int i);

/* BEGIN static implementation */

int ledger_act_path_push_string
  (unsigned char* buf, int len, char const* string)
{
  int i;
  int string_length = 0;
  for (i = 0; i < len; ++i){
    if (string[i] == 0) break;
    buf[i] = string[i];
    string_length += 1;
  }
  for (; string[i] != 0; ++i){
    string_length += 1;
  }
  return string_length;
}

int ledger_act_path_push_int
  (unsigned char* buf, int len, int n)
{
  unsigned char number_text[sizeof(int)*CHAR_BIT/2+1];
  ledger_util_itoa(n, number_text, sizeof(number_text), 0);
  return ledger_act_path_push_string(buf, len, (char*)number_text);
}

int ledger_act_path_push_token
  ( unsigned char* buf, int len, int typ,
    unsigned char const* name, int item_id, int i)
{
  int out = 0;
  switch (typ){
  case LEDGER_ACT_PATH_LEDGER:
    if (out < len) out += ledger_act_path_push_string
        (buf+out,len-out,"ledger");
    else out += 6;
    break;
  case LEDGER_ACT_PATH_JOURNAL:
    if (out < len) out += ledger_act_path_push_string
        (buf+out,len-out,"journal");
    else out += 7;
    break;
  case LEDGER_ACT_PATH_ACCOUNT:
    if (out < len) out += ledger_act_path_push_string
        (buf+out,len-out,"account");
    else out += 7;
    break;
  case LEDGER_ACT_PATH_ENTRY:
    if (out < len) out += ledger_act_path_push_string
        (buf+out,len-out,"entry");
    else out += 5;
    break;
  }
  if (name != NULL){
    if (out < len) buf[out] = ':';
    out += 1;
    if (out < len) out += ledger_act_path_push_string
        (buf+out, len-out, (char const*)name);
    else out += ledger_util_ustrlen(name);
  } else if (item_id >= 0){
    if (out < len) buf[out] = '#';
    out += 1;
    if (out < len) out += ledger_act_path_push_int
        (buf+out, len-out, item_id);
    else out += ledger_util_itoa(item_id, NULL, 0, 0);
  } else {
    if (out < len) buf[out] = '@';
    out += 1;
    if (out < len) out += ledger_act_path_push_int
        (buf+out, len-out, i);
    else out += ledger_util_itoa(i, NULL, 0, 0);
  }
  return out;
}

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

int ledger_act_path_render
  ( unsigned char* buf, int len, struct ledger_act_path path,
    struct ledger_book const* book)
{
  int out = 0;
  switch (path.typ){
  case LEDGER_ACT_PATH_BOOK:
    {
      if (out < len){
        buf[out] = '/';
      }
      out += 1;
    }break;
  case LEDGER_ACT_PATH_LEDGER:
    {
      struct ledger_ledger const* ledger;
      unsigned char const* name_ledger = NULL;
      int item_id_ledger = -1;
      ledger = ledger_book_get_ledger_c(book, path.path[0]);
      if (ledger != NULL){
        name_ledger = ledger_ledger_get_name(ledger);
        item_id_ledger = ledger_ledger_get_id(ledger);
      }
      if (out < len) buf[out] = '/';
      out += 1;
      if (out < len) out += ledger_act_path_push_token
          ( buf+out, len-out, LEDGER_ACT_PATH_LEDGER,
            name_ledger, item_id_ledger, path.path[0]);
      else out += ledger_act_path_push_token
          ( NULL, 0, LEDGER_ACT_PATH_LEDGER,
            name_ledger, item_id_ledger, path.path[0]);
    }break;
  case LEDGER_ACT_PATH_JOURNAL:
    {
      struct ledger_journal const* ledger;
      unsigned char const* name_journal = NULL;
      int item_id_journal = -1;
      ledger = ledger_book_get_journal_c(book, path.path[0]);
      if (ledger != NULL){
        name_journal = ledger_journal_get_name(ledger);
        item_id_journal = ledger_journal_get_id(ledger);
      }
      if (out < len) buf[out] = '/';
      out += 1;
      if (out < len) out += ledger_act_path_push_token
          ( buf+out, len-out, LEDGER_ACT_PATH_JOURNAL,
            name_journal, item_id_journal, path.path[0]);
      else out += ledger_act_path_push_token
          ( NULL, 0, LEDGER_ACT_PATH_JOURNAL,
            name_journal, item_id_journal, path.path[0]);
    }break;
  case LEDGER_ACT_PATH_ACCOUNT:
    {
      struct ledger_ledger const* ledger;
      unsigned char const* name_ledger = NULL;
      int item_id_ledger = -1;
      struct ledger_account const* account;
      unsigned char const* name_account = NULL;
      int item_id_account = -1;
      ledger = ledger_book_get_ledger_c(book, path.path[0]);
      if (ledger != NULL){
        name_ledger = ledger_ledger_get_name(ledger);
        item_id_ledger = ledger_ledger_get_id(ledger);
        account = ledger_ledger_get_account_c(ledger, path.path[1]);
      } else account = NULL;
      if (account != NULL){
        name_account = ledger_account_get_name(account);
        item_id_account = ledger_account_get_id(account);
      }
      if (out < len) buf[out] = '/';
      out += 1;
      if (out < len) out += ledger_act_path_push_token
          ( buf+out, len-out, LEDGER_ACT_PATH_LEDGER,
            name_ledger, item_id_ledger, path.path[0]);
      else out += ledger_act_path_push_token
          ( NULL, 0, LEDGER_ACT_PATH_LEDGER,
            name_ledger, item_id_ledger, path.path[0]);
      if (out < len) buf[out] = '/';
      out += 1;
      if (out < len) out += ledger_act_path_push_token
          ( buf+out, len-out, LEDGER_ACT_PATH_ACCOUNT,
            name_account, item_id_account, path.path[1]);
      else out += ledger_act_path_push_token
          ( NULL, 0, LEDGER_ACT_PATH_ACCOUNT,
            name_account, item_id_account, path.path[1]);
    }break;
  case LEDGER_ACT_PATH_ENTRY:
    {
      struct ledger_journal const* journal;
      unsigned char const* name_journal = NULL;
      int item_id_journal = -1;
      struct ledger_entry const* entry;
      unsigned char const* name_entry = NULL;
      int item_id_entry = -1;
      journal = ledger_book_get_journal_c(book, path.path[0]);
      if (journal != NULL){
        name_journal = ledger_journal_get_name(journal);
        item_id_journal = ledger_journal_get_id(journal);
        entry = ledger_journal_get_entry_c(journal, path.path[1]);
      } else entry = NULL;
      if (entry != NULL){
        name_entry = ledger_entry_get_name(entry);
        item_id_entry = ledger_entry_get_id(entry);
      }
      if (out < len) buf[out] = '/';
      out += 1;
      if (out < len) out += ledger_act_path_push_token
          ( buf+out, len-out, LEDGER_ACT_PATH_JOURNAL,
            name_journal, item_id_journal, path.path[0]);
      else out += ledger_act_path_push_token
          ( NULL, 0, LEDGER_ACT_PATH_JOURNAL,
            name_journal, item_id_journal, path.path[0]);
      if (out < len) buf[out] = '/';
      out += 1;
      if (out < len) out += ledger_act_path_push_token
          ( buf+out, len-out, LEDGER_ACT_PATH_ENTRY,
            name_entry, item_id_entry, path.path[1]);
      else out += ledger_act_path_push_token
          ( NULL, 0, LEDGER_ACT_PATH_ENTRY,
            name_entry, item_id_entry, path.path[1]);
    }break;
  }
  /* terminate the string */
  if (out < len) buf[out] = 0;
  else if (len > 0) buf[len-1] = 0;
  return out;
}


struct ledger_act_path ledger_act_path_compute
  ( struct ledger_book const* book, unsigned char const* path_text,
    struct ledger_act_path start, int *ok)
{
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
              if (ledger_util_ustrncmp(
                (unsigned char const*)"ledger:",token_point,7) == 0)
              {
                unsigned char const* next_name = token_point+7;
                next_index = ledger_find_ledger_by_name(book, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_LEDGER;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"ledger#",token_point,7) == 0)
              {
                int next_id = ledger_util_atoi(token_point+7);
                next_index = ledger_find_ledger_by_id(book, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_LEDGER;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"ledger@",token_point,7) == 0)
              {
                next_index = ledger_util_atoi(token_point+7);
                if (next_index < 0
                ||  next_index >= ledger_book_get_ledger_count(book))
                {
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_LEDGER;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"journal:",token_point,8) == 0)
              {
                unsigned char const* next_name = token_point+8;
                next_index = ledger_find_journal_by_name(book, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_JOURNAL;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"journal#",token_point,8) == 0)
              {
                int next_id = ledger_util_atoi(token_point+8);
                next_index = ledger_find_journal_by_id(book, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_JOURNAL;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"journal@",token_point,8) == 0)
              {
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
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"account:",token_point,8) == 0)
              {
                unsigned char const* next_name = token_point+8;
                next_index = ledger_find_account_by_name(ledger, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ACCOUNT;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"account#",token_point,8) == 0)
              {
                int next_id = ledger_util_atoi(token_point+8);
                next_index = ledger_find_account_by_id(ledger, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ACCOUNT;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"account@",token_point,8) == 0)
              {
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
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"entry:",token_point,6) == 0)
              {
                unsigned char const* next_name = token_point+6;
                next_index = ledger_find_entry_by_name(journal, next_name);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ENTRY;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"entry#",token_point,6) == 0)
              {
                int next_id = ledger_util_atoi(token_point+6);
                next_index = ledger_find_entry_by_id(journal, next_id);
                if (next_index < 0){
                  completion = 0;
                  break;
                }
                next_typ = LEDGER_ACT_PATH_ENTRY;
              } else if (ledger_util_ustrncmp(
                (unsigned char const*)"entry@",token_point,6) == 0)
              {
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
