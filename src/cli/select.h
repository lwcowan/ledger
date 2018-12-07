/*
 * file: cli/select.h
 * brief: Line selection commands
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Select_H__
#define __Ledger_cli_Select_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;

/*
 * Select transaction lines fulfilling certain requirements.
 */
int ledger_cli_select(struct ledger_cli_line *tracking, int argc, char **argv);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Select_H__*/
