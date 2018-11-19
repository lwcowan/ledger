
#include "table.h"
#include "../base/util.h"
#include "../base/table.h"
#include "../../deps/minicsv/minicsv.h"
#include <string.h>
#include <limits.h>


/*
 * Escape a row field.
 * - mark row mark
 * - column column index
 * - remaining bytes remaining
 * - output output buffer
 * @return the size in bytes
 */
static size_t ledger_io_table_escape_text
  ( struct ledger_table_mark const* mark, int column,
    size_t remaining, unsigned char* output);


/* BEGIN static implementation */

size_t ledger_io_table_escape_text
  ( struct ledger_table_mark const* mark, int column,
    size_t remaining, unsigned char* output)
{
  size_t char_count = 0, with_quotes = 0;
  unsigned char buf[256];
  unsigned char *active_buffer = buf;
  size_t active_length = sizeof(buf);
  int const real_length =
      ledger_table_fetch_string(mark,column,buf,sizeof(buf));
  if (real_length < 0) return 0;
  else if (real_length >= 256){
    active_buffer = (unsigned char*)ledger_util_malloc(real_length+1);
    if (active_buffer == NULL){
      /* forget it */
      return 0;
    } else {
      active_length = real_length;
      ledger_table_fetch_string(mark,column,active_buffer,real_length+1);
    }
  } else active_length = real_length;
  /* scan for escapeable characters */{
    size_t i;
    for (i = 0; i < active_length; ++i){
      switch (active_buffer[i]){
      case '"':
        with_quotes = 2;
        char_count += 2;/* double double quotes */
        break;
      case '\n':
      case '\r':
      case ',':
        with_quotes = 2;
        char_count += 1;
        break;
      default:
        char_count += 1;
        break;
      }
    }
  }
  /* render the CSV item */if (output != NULL){
    size_t i, write_point = 0;
    if (with_quotes){
      if (write_point < remaining){
        output[write_point++] = '"';
      }
    }
    for (i = 0; i < active_length; ++i){
      switch (active_buffer[i]){
      case '"':
        if (write_point < remaining) output[write_point++] = '"';
        if (write_point < remaining) output[write_point++] = '"';
        break;
      default:
        if (write_point < remaining) output[write_point++] = active_buffer[i];
        break;
      }
    }
    if (with_quotes){
      if (write_point < remaining){
        output[write_point++] = '"';
      }
    }
  }
  if (active_buffer != buf){
    ledger_util_free(active_buffer);
  }
  return char_count + with_quotes;
}

/* END   static implementation */


/* BEGIN implementation */

int ledger_io_table_parse_csv(struct ledger_table* table, unsigned char *csv){
  int ok = 1;
  int const column_count = ledger_table_get_column_count(table);
  char *running = (char*)csv;
  char** token_lines;
  struct ledger_table_mark *mark;
  if (column_count == 0) return 1;
  /* allocate the token pointers */{
    token_lines = (char**)ledger_util_malloc(column_count*sizeof(char*));
    if (token_lines == NULL){
      return 0;
    }
  }
  mark = ledger_table_end(table);
  while (*running){
    size_t real_column_count;
    if (!ledger_table_add_row(mark)){
      ok = 0;break;
    }
    running = minicsv_parse_line
      (running, token_lines, &real_column_count, column_count);
    if (real_column_count != column_count){
      ok = 0;break;
    }
    /* fill in the fields */{
      int i;
      for (i = 0; i < column_count; ++i){
        ok = ledger_table_put_string
          (mark, i, (unsigned char*)token_lines[i]);
        if (!ok) break;
      }
      if (!ok) break;
    }
    /* move to end of table */
    ledger_table_mark_move(mark, +1);
  }
  ledger_table_mark_free(mark);
  /* free the token pointers */{
    ledger_util_free(token_lines);
  }
  return ok;
}

unsigned char* ledger_io_table_print_csv(struct ledger_table const* table){
  int const column_count = ledger_table_get_column_count(table);
  size_t entire_csv_length = 0;
  unsigned char* print_paper;
  /* compute the entire csv length */{
    size_t row_point = 0;
    struct ledger_table_mark *mark = ledger_table_begin_c(table);
    struct ledger_table_mark *end_mark = ledger_table_end_c(table);
    for (; !ledger_table_mark_is_equal(mark, end_mark);
          ledger_table_mark_move(mark, +1))
    {
      int i;
      if (row_point > 0){
        /* put a new line */entire_csv_length += 1;
      }
      for (i = 0; i < column_count; ++i){
        if (i > 0) entire_csv_length += 1;/*comma*/
        entire_csv_length += ledger_io_table_escape_text(mark,i,0,NULL);
      }
      row_point += 1;
    }
    ledger_table_mark_free(end_mark);
    ledger_table_mark_free(mark);
  }
  /* allocate the entire text */{
    if (entire_csv_length >= ~0u){
      /* avoid overflow */
      return NULL;
    }
    print_paper = (unsigned char*)ledger_util_malloc(entire_csv_length+1);
    if (print_paper == NULL){
      /* give up and */return NULL;
    }
  }
  /* render the entire csv */{
    size_t row_point = 0;
    size_t write_point = 0;
    struct ledger_table_mark *mark = ledger_table_begin_c(table);
    struct ledger_table_mark *end_mark = ledger_table_end_c(table);
    for (; !ledger_table_mark_is_equal(mark, end_mark);
          ledger_table_mark_move(mark, +1))
    {
      int i;
      if (write_point >= entire_csv_length) break;
      if (row_point > 0){
        print_paper[write_point++] = '\n';/*put a new line*/
        if (write_point >= entire_csv_length) break;
      }
      for (i = 0; i < column_count; ++i){
        if (write_point >= entire_csv_length) break;
        if (i > 0){
          print_paper[write_point++] = ',';/*comma*/
          if (write_point >= entire_csv_length) break;
        }
        write_point += ledger_io_table_escape_text
          (mark,i,entire_csv_length-write_point,print_paper+write_point);
      }
      row_point += 1;
    }
    ledger_table_mark_free(end_mark);
    ledger_table_mark_free(mark);
    print_paper[write_point] = 0;
  }
  return print_paper;
}

/* END   implementation */
