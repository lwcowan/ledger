
#include "select.h"
#include "../base/table.h"
#include "../base/util.h"


/*
 * Check a table row against a condition.
 * - m a mark pointing to the row to check
 * - cnd the condition to check
 * @return nonzero if the check passes, zero otherwise
 */
int ledger_select_check_cond
  ( struct ledger_table_mark* m, struct ledger_select_cond cnd);

/*
 * Check a table row against a condition.
 * - m a mark pointing to the row to check
 * - i field index
 * - buf buffer to use if big enough
 * - len length of buffer
 * @return a possibly-allocated string on success,
 */
unsigned char* ledger_select_buf_fetch_str
  ( struct ledger_table_mark* m, int i, unsigned char* buf, int len);

/*
 * Free a possibly-allocated string.
 * - ptr string to free
 * - buf static buffer to use if big enough
 */
void ledger_select_buf_free
  (unsigned char* ptr, unsigned char* buf);



/* BEGIN static implementation */

unsigned char* ledger_select_buf_fetch_str
  ( struct ledger_table_mark* m, int i, unsigned char* buf, int len)
{
  int used_length;
  used_length = ledger_table_fetch_string(m, i, buf, len);
  if (used_length >= len){
    unsigned char* ptr;
    ptr = ledger_util_malloc(used_length+1);
    if (ptr == NULL) return NULL;
    else {
      ledger_table_fetch_string(m, i, ptr, used_length+1);
      return ptr;
    }
  } else return buf;
}

void ledger_select_buf_free(unsigned char* ptr, unsigned char* buf){
  if (ptr != buf)
    ledger_util_free(ptr);
  return;
}

int ledger_select_check_cond
  ( struct ledger_table_mark* m, struct ledger_select_cond cnd)
{
  int yes;
  switch (cnd.cmp){
  case LEDGER_SELECT_EQUAL:
    {
      unsigned char text_stash[64];
      unsigned char* text = ledger_select_buf_fetch_str
        (m,cnd.column, text_stash, sizeof(text_stash));
      if (text == NULL) yes = -1;
      else {
        yes = (ledger_util_ustrcmp(text, cnd.value)==0) ? 1 : 0;
        ledger_select_buf_free(text, text_stash);
      }
    }break;
  case LEDGER_SELECT_LESS:
    {
      unsigned char text_stash[64];
      unsigned char* text = ledger_select_buf_fetch_str
        (m,cnd.column, text_stash, sizeof(text_stash));
      if (text == NULL) yes = -1;
      else {
        yes = (ledger_util_ustrcmp(text, cnd.value)<0) ? 1 : 0;
        ledger_select_buf_free(text, text_stash);
      }
    }break;
  case LEDGER_SELECT_MORE:
    {
      unsigned char text_stash[64];
      unsigned char* text = ledger_select_buf_fetch_str
        (m,cnd.column, text_stash, sizeof(text_stash));
      if (text == NULL) yes = -1;
      else {
        yes = (ledger_util_ustrcmp(text, cnd.value)>0) ? 1 : 0;
        ledger_select_buf_free(text, text_stash);
      }
    }break;
  case LEDGER_SELECT_NOTEQUAL:
    {
      unsigned char text_stash[64];
      unsigned char* text = ledger_select_buf_fetch_str
        (m,cnd.column, text_stash, sizeof(text_stash));
      if (text == NULL) yes = -1;
      else {
        yes = (ledger_util_ustrcmp(text, cnd.value)!=0) ? 1 : 0;
        ledger_select_buf_free(text, text_stash);
      }
    }break;
  case LEDGER_SELECT_NOTLESS:
    {
      unsigned char text_stash[64];
      unsigned char* text = ledger_select_buf_fetch_str
        (m,cnd.column, text_stash, sizeof(text_stash));
      if (text == NULL) yes = -1;
      else {
        yes = (ledger_util_ustrcmp(text, cnd.value)>=0) ? 1 : 0;
        ledger_select_buf_free(text, text_stash);
      }
    }break;
  case LEDGER_SELECT_NOTMORE:
    {
      unsigned char text_stash[64];
      unsigned char* text = ledger_select_buf_fetch_str
        (m,cnd.column, text_stash, sizeof(text_stash));
      if (text == NULL) yes = -1;
      else {
        yes = (ledger_util_ustrcmp(text, cnd.value)<=0) ? 1 : 0;
        ledger_select_buf_free(text, text_stash);
      }
    }break;
  default:
    {
      yes = 0;
    }break;
  }
  return yes;
}

/* END   static implementation */

/* BEGIN implementation */

int ledger_select_by_cond
  ( struct ledger_table* t, void* arg, ledger_select_cb cb,
    int len, struct ledger_select_cond const cond[], int dir)
{
  int result = 0;
  struct ledger_table_mark* cur, * end;
  int used_direction;
  if (dir < 0){
    cur = ledger_table_end(t);
    if (cur != NULL)
      ledger_table_mark_move(cur, -1);
    used_direction = -1;
  } else {
    cur = ledger_table_begin(t);
    used_direction = +1;
  }
  end = ledger_table_end(t);
  if (cur == NULL || end == NULL){
    return -1;
  } else for (; !ledger_table_mark_is_equal(cur, end);
        ledger_table_mark_move(cur, used_direction))
  {
    int cond_i;
    int yes = 1;
    for (cond_i = 0; yes == 1 && cond_i < len; ++cond_i){
      yes = ledger_select_check_cond(cur, cond[cond_i]);
    }
    if (yes == 1){
      result = (*cb)(arg, cur);
      if (result != 0) break;
    } else if (yes == -1){
      result = -1;
    }
  }
  return result;
}


int ledger_select_by_cond_c
  ( struct ledger_table const* t, void* arg, ledger_select_cb cb,
    int len, struct ledger_select_cond const cond[], int dir)
{
  int result = 0;
  struct ledger_table_mark* cur, * end;
  int used_direction;
  if (dir < 0){
    cur = ledger_table_end_c(t);
    if (cur != NULL)
      ledger_table_mark_move(cur, -1);
    used_direction = -1;
  } else {
    cur = ledger_table_begin_c(t);
    used_direction = +1;
  }
  end = ledger_table_end_c(t);
  if (cur == NULL || end == NULL){
    return -1;
  } else for (; !ledger_table_mark_is_equal(cur, end);
        ledger_table_mark_move(cur, used_direction))
  {
    int cond_i;
    int yes = 1;
    for (cond_i = 0; yes == 1 && cond_i < len; ++cond_i){
      yes = ledger_select_check_cond(cur, cond[cond_i]);
    }
    if (yes == 1){
      result = (*cb)(arg, cur);
      if (result != 0) break;
    } else if (yes == -1){
      result = -1;
    }
  }
  return result;
}

/* END   implementation */
