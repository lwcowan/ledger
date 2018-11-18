
#include "line.h"
#include <stddef.h>
#include "../../deps/linenoise/linenoise.h"

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
  return 0;
}

void ledger_cli_free_line(struct ledger_cli_line *tracking, char* line){
  linenoiseFree(line);
  return;
}
