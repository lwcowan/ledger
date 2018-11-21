/*
 * file: cli/iocmd.h
 * brief: Transport commands
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_IOCmd_H__
#define __Ledger_cli_IOCmd_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


struct ledger_cli_line;

/*
 * Load a book.
 */
int ledger_cli_read(struct ledger_cli_line *tracking, int argc, char **argv);

/*
 * Save a book.
 */
int ledger_cli_write(struct ledger_cli_line *tracking, int argc, char **argv);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_IOCmd_H__*/
