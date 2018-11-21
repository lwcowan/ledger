/*
 * file: cli/test.h
 * brief: Test for command line
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Test_H__
#define __Ledger_cli_Test_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;

/*
 * Test the command line.
 */
int ledger_cli_test(struct ledger_cli_line *tracking, int argc, char **argv);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Test_H__*/
