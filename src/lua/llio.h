/*
 * file: act/llio.h
 * brief: Lua extension for input/output library API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_lua_LLIo_H__
#define __Ledger_lua_LLIo_H__


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/*
 * Lua state
 */
struct lua_State;

/*
 * Open the input/output library.
 * @param l native lua state
 */
void ledger_luaopen_io(struct lua_State *l);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_lua_LLIo_H__*/
