/*
 * file: cli/rename.h
 * brief: Renaming commands
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Rename_H__
#define __Ledger_cli_Rename_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;

/*
 * Rename the current object.
 */
int ledger_cli_rename(struct ledger_cli_line *tracking, int argc, char **argv);



#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Rename_H__*/
