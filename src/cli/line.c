
#include "line.h"
#include <stddef.h>
#include "../../deps/linenoise/linenoise.h"
#include <stdio.h>
#include <string.h>
#include "../base/util.h"
#include "../base/book.h"
#include <stdlib.h>
#include "../act/arg.h"

#include "test.h"
#include "quit.h"
#include "iocmd.h"
#include "navigate.h"
#include "manage.h"
#include "rename.h"
#include "select.h"


struct ledger_cli_token {
  ledger_cli_cb fn;
  char const* name;
};
static struct ledger_cli_token const ledger_cli_cb_list[] = {
  { ledger_cli_test,  "test" },
  { ledger_cli_quit,  "quit" },
  { ledger_cli_read,  "read" },
  { ledger_cli_write, "write" },
  { ledger_cli_list,  "list" },
  { ledger_cli_enter, "enter" },
  { ledger_cli_info, "info" },
  { ledger_cli_select, "select" },
  { ledger_cli_rename,  "rename" },
  { ledger_cli_make_ledger, "make_ledger" },
  { ledger_cli_make_journal, "make_journal" },
  { ledger_cli_make_account, "make_account" },
  { ledger_cli_make_entry, "make_entry" }
};


int ledger_cli_line_init(struct ledger_cli_line *tracking){
  struct ledger_book *book = ledger_book_new();
  if (book == NULL) return 0;
  tracking->done = 0;
  tracking->first_quit = 0;
  tracking->book = book;
  tracking->object_path = ledger_act_path_root();
  return 1;
}

void ledger_cli_line_clear(struct ledger_cli_line *tracking){
  ledger_book_free(tracking->book);
}

char* ledger_cli_get_line(struct ledger_cli_line *tracking){
  if (tracking->done) return NULL;
  else {
    char* text = linenoise("ledger-cli> ");
    if (text == NULL) tracking->done = 1;
    return text;
  }
}

char* ledger_cli_get_sub_line
  (struct ledger_cli_line *tracking, char const* prompt)
{
  if (tracking->done) return NULL;
  else {
    char* text = linenoise(prompt);
    if (text == NULL) tracking->done = 1;
    return text;
  }
}

void ledger_cli_set_history_len(struct ledger_cli_line *tracking, int len){
  linenoiseHistorySetMaxLen(len);
  return;
}

int ledger_cli_do_line
  (struct ledger_cli_line *tracking, char const* command)
{
  int result = 0;
  int token_count;
  struct ledger_arg_list *pieces;
  char** total_pieces;
  /* pass zero: comment */
  if (command[0] == '#') return 0;
  pieces = ledger_arg_list_new();
  if (pieces == NULL){
    fprintf(stderr, "Error encountered with allocating argument list.\n");
    ledger_arg_list_free(pieces);
    return EXIT_FAILURE;
  }
  result = ledger_arg_list_parse(pieces, command);
  if (result == 0){
    fprintf(stderr, "Error encountered with parsing command line.\n");
    ledger_arg_list_free(pieces);
    return 0;
  }
  total_pieces = (char**)ledger_arg_list_fetch(pieces);
  token_count = ledger_arg_list_get_count(pieces)-1;
  /* third pass: process the arguments */if (token_count > 0
      && total_pieces[0] != NULL)
  {
    int i;
    int const cb_length = sizeof(ledger_cli_cb_list)/
      sizeof(ledger_cli_cb_list[0]);
    for (i = 0; i < cb_length; ++i){
      if (strcmp(ledger_cli_cb_list[i].name, total_pieces[0]) == 0){
        /* choose this command and */break;
      }
    }
    /* use a command */if (i < cb_length){
      linenoiseHistoryAdd(command);
      result =
        (*ledger_cli_cb_list[i].fn)(tracking, token_count, total_pieces);
    } else {
      fprintf(stderr,"Unrecognized command: \"%s\"\n", total_pieces[0]);
      result = EXIT_FAILURE;
    }
  }
  /* fourth pass: free up the arguments */{
    ledger_arg_list_free(pieces);
  }
  return 0;
}

void ledger_cli_free_line(struct ledger_cli_line *tracking, char* line){
  linenoiseFree(line);
  return;
}
