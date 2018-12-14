/*
 * file: cli/print.h
 * brief: Transaction line printing API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Print_H__
#define __Ledger_cli_Print_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;
struct ledger_table_mark;

/*
 * Print a transaction line.
 * - tracking line tracking structure
 * - m account table mark to print
 * - f `stdio` file handle
 * @return nonzero on success, zero otherwise
 */
int ledger_cli_print_account_line
  ( struct ledger_cli_line *tracking,
    struct ledger_table_mark const* m, FILE* f);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Print_H__*/
