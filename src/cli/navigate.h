/*
 * file: cli/navigate.h
 * brief: Navigation commands
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Navigate_H__
#define __Ledger_cli_Navigate_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;

/*
 * List the contents of the current object.
 */
int ledger_cli_list(struct ledger_cli_line *tracking, int argc, char **argv);

/*
 * Enter a child object.
 */
int ledger_cli_enter(struct ledger_cli_line *tracking, int argc, char **argv);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Navigate_H__*/
