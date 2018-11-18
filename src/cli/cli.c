
#include "../base/book.h"
#include "line.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  int all_result = 0;
  struct ledger_book *active_book;
  /* construct a book */
  active_book = ledger_book_new();
  if (active_book == NULL){
    fprintf(stderr,"Failed to initialize the book structure.\n");
    return EXIT_FAILURE;
  }
  /* enter processing loop */{
    struct ledger_cli_line line_tracking = {0,0,active_book};
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
  ledger_book_free(active_book);
  return all_result;
}
