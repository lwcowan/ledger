/*
 * file: cli/line.h
 * brief: Line processing API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Line_H__
#define __Ledger_cli_Line_H__

#include "../act/path.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_book;

struct ledger_cli_line {
  /* flag set to nonzero when it is time to quit */
  int done;
  /* nonzero if quit on first error */
  int first_quit;
  /* active book */
  struct ledger_book* book;
  /* path to current object */
  struct ledger_act_path object_path;
};


/*
 * Command line callback.
 * - tracking line tracking structure
 * - argc number of arguments
 * - argv argument texts
 * @return zero on success, nonzero otherwise
 */
typedef int (*ledger_cli_cb)
  (struct ledger_cli_line *tracking, int argc, char **argv);


/*
 * Initialize the line tracking structure.
 * - tracking line tracking structure
 */
int ledger_cli_line_init(struct ledger_cli_line *tracking);

/*
 * Clear the line tracking structure.
 * - tracking line tracking structure
 */
void ledger_cli_line_clear(struct ledger_cli_line *tracking);

/*
 * Get a line of text from the input stream.
 * - tracking line tracking structure
 * @return the text, or NULL at end of stream
 */
char* ledger_cli_get_line(struct ledger_cli_line *tracking);

/*
 * Set a length for the history.
 * - tracking line tracking structure
 * - len new history length
 */
void ledger_cli_set_history_len(struct ledger_cli_line *tracking, int len);

/*
 * Process a command line.
 * - tracking line tracking structure
 * - command command to process
 * @return zero on success, nonzero otherwise
 */
int ledger_cli_do_line
  (struct ledger_cli_line *tracking, char const* command);

/*
 * Free a line of text from the input stream.
 * - tracking line tracking structure
 * @return the text, or NULL at end of stream
 */
void ledger_cli_free_line(struct ledger_cli_line *tracking, char* line);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Line_H__*/
