/*
 * file: act/llbasetable.h
 * brief: Lua extension for base library API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_lua_LLBaseTable_H__
#define __Ledger_lua_LLBaseTable_H__


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/*
 * Lua state
 */
struct lua_State;

/*
 * Open the table-handling portion of the base library.
 * @param l native lua state
 */
void ledger_luaopen_basetable(struct lua_State *l);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_lua_LLBaseTable_H__*/
