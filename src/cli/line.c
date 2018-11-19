
#include "line.h"
#include <stddef.h>
#include "../../deps/linenoise/linenoise.h"
#include <stdio.h>
#include <string.h>
#include "../base/util.h"
#include <stdlib.h>

#include "test.h"
#include "quit.h"


struct ledger_cli_token {
  ledger_cli_cb fn;
  char const* name;
};
static struct ledger_cli_token const ledger_cli_cb_list[] = {
  { ledger_cli_test, "test" },
  { ledger_cli_quit, "quit" }
};

char* ledger_cli_get_line(struct ledger_cli_line *tracking){
  return linenoise("ledger-cli> ");
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
  char** total_pieces;
  /* pass zero: comment */
  if (command[0] == '#') return 0;
  /* first pass: count the tokens */{
    char const* p;
    int quote = 0;
    int alpha_count = 0;
    for (p = command; *p; ++p){
      if (quote){
        if (*p == '"')
          quote = 0;
          alpha_count += 1;
      } else {
        if (*p == '"'){
          alpha_count += 1;
          quote = 1;
        } else if (*p == ' ') {
          if (alpha_count > 0){
            token_count += 1;
            alpha_count = 0;
          }
        } else {
          alpha_count += 1;
        }
      }
    }
    if (alpha_count > 0){
      token_count += 1;
    }
    total_pieces = ledger_util_malloc(sizeof(char*)*(token_count+1));
    if (total_pieces == NULL){
      fprintf(stderr, "Failed to allocate token piece array.\n");
    } else total_pieces[token_count] = NULL;
  }
  /* second pass: extract the tokens */{
    char const* p;
    char const* token_start = command;
    int quote = 0;
    int ok = 1;
    int micro_token_count = 0;
    int alpha_count = 0;
    for (p = command; *p; ++p){
      if (quote){
        if (*p == '"') {
          quote = 0;
          alpha_count += 1;
        } else {
          alpha_count += 1;
        }
      } else {
        if (*p == '"'){
          alpha_count += 1;
          quote = 1;
        } else if (*p == ' ') {
          if (alpha_count > 0){
            total_pieces[micro_token_count] =
              ledger_util_ustrndup(token_start, alpha_count, &ok);
            if (!ok) break;
            micro_token_count += 1;
            alpha_count = 0;
          }
          token_start = p+1;
          if (micro_token_count >= token_count) break;
        } else {
          alpha_count += 1;
        }
      }
    }
    if (ok && alpha_count > 0 && micro_token_count < token_count){
      total_pieces[micro_token_count] =
        ledger_util_ustrndup(token_start, alpha_count, &ok);
      if (ok) micro_token_count += 1;
    }
    if (!ok){
      int i;
      for (i = 0; i < micro_token_count; ++i){
        ledger_util_free(total_pieces[i]);
      }
      fprintf(stderr, "Failed to allocate token pieces.\n");
      return 0;
    } else token_count = micro_token_count;
  }
  /* third pass: process the arguments */if (total_pieces[0] != NULL){
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
    int i;
    for (i = 0; i < token_count; ++i){
      ledger_util_free(total_pieces[i]);
    }
  }
  return 0;
}

void ledger_cli_free_line(struct ledger_cli_line *tracking, char* line){
  linenoiseFree(line);
  return;
}
