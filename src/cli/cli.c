
#include "../base/book.h"
#include "line.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  int all_result = 0;
  struct ledger_cli_line line_tracking;
  if (!ledger_cli_line_init(&line_tracking)){
    fprintf(stderr,"Failed to initialize the line tracking structure.\n");
    return EXIT_FAILURE;
  }
  /* enter processing loop */{
    ledger_cli_set_history_len(&line_tracking, 10);
    int result = 0;
    while (!line_tracking.done){
      char *line = ledger_cli_get_line(&line_tracking);
      if (line == NULL){
        line_tracking.done = 1;
        break;
      } else {
        result = ledger_cli_do_line(&line_tracking, line);
        if (result != 0){
          if (all_result == 0)
            all_result = result;
          if (line_tracking.first_quit)
            line_tracking.done = 1;
        }
        ledger_cli_free_line(&line_tracking, line);
      }
    }
  }
  ledger_cli_line_clear(&line_tracking);
  return all_result;
}
