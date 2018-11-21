
#include "test.h"
#include <stdio.h>

int ledger_cli_test(struct ledger_cli_line *tracking, int argc, char **argv){
  int i;
  for (i = 0; i < argc; ++i){
    fprintf(stdout, "argv[%i] = \"%s\"\n",i,argv[i]);
  }
  return 0;
}
