
#include "quit.h"
#include "line.h"
#include <stdio.h>

int ledger_cli_quit(struct ledger_cli_line *tracking, int argc, char **argv){
  tracking->done = 1;
  fputs("Bye.\n",stderr);
  return 0;
}
