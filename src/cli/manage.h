/*
 * file: cli/manage.h
 * brief: Management commands
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_cli_Manage_H__
#define __Ledger_cli_Manage_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_cli_line;


/*
 * Make a ledger.
 */
int ledger_cli_make_ledger
  (struct ledger_cli_line *tracking, int argc, char **argv);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_cli_Manage_H__*/
