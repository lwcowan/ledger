/*
 * file: cli/quit.h
 * brief: Quit for command line
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Quit_H__
#define __Ledger_cli_Quit_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;

/*
 * Quit the command line.
 */
int ledger_cli_quit(struct ledger_cli_line *tracking, int argc, char **argv);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Quit_H__*/
