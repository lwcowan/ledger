/*
 * file: act/luaext.h
 * brief: Lua extension API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_act_LuaExt_H__
#define __Ledger_act_LuaExt_H__


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * Lua state
 */
struct ledger_lua;

/*
 * Create a new Lua state for `ledger`.
 * @return the new state on success, NULL otherwise
 */
struct ledger_lua* ledger_lua_new(void);

/*
 * Free a Lua state for `ledger`.
 * - l the state to free
 */
void ledger_lua_close(struct ledger_lua* l);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_LuaExt_H__*/
