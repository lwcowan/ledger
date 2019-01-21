/*
 * file: lua/llact.h
 * brief: Lua extension for action library API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_lua_LLAct_H__
#define __Ledger_lua_LLAct_H__

#include "../act/path.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_book;
struct ledger_transaction;

/*
 * Lua state
 */
struct lua_State;


/*
 * Post the path on the top of the stack.
 * - L lua state to modify
 * - path path to post
 * - book source book, reference to be surrendered to the path table
 * - ok if nonzero, the path is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llact_postpath
  ( struct lua_State *L, struct ledger_act_path path,
    struct ledger_book* book, int ok, char const* err);

/*
 * Read the table from on top of the stack as a ledger path.
 * - L lua state to modify
 * - path pointer to path struct to overwrite
 * - book pointer to receive a reference to the source book
 * - throwing whether to throw on error
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llact_getpath
  ( struct lua_State *L, struct ledger_act_path *path,
    struct ledger_book** book, int throwing, char const* err);

/*
 * Post the given transaction to the top of the stack.
 * - L lua state to modify
 * - a transaction to post
 * - ok if nonzero, the transaction is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llact_posttransaction
  ( struct lua_State *L, struct ledger_transaction* a,
    int ok, char const* err);

/*
 * Open the action library.
 * @param l native lua state
 */
void ledger_luaopen_act(struct lua_State *l);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_lua_LLBase_H__*/
