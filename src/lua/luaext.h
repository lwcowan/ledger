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

/*
 * Execute a Lua script on the ledger-lua state.
 * - l the state to use
 * - name a name for the string
 * - s string to execute
 * @return one on success, zero otherwise
 */
int ledger_lua_exec_str(struct ledger_lua* l,
    unsigned char const* name, unsigned char const* s);

/*
 * Add libraries to Lua for standalone processing.
 * - l the state to configure
 * @return one on success, zero otherwise
 */
int ledger_lua_openlibs(struct ledger_lua* l);

/*
 * Execute a Lua script on the ledger-lua state.
 * - l the state to use
 * - name a name for the string
 * - f name of file to execute
 * @return one on success, zero otherwise
 */
int ledger_lua_exec_file(struct ledger_lua* l,
    unsigned char const* name, char const* f);

/*
 * Set command-line arguments for a ledger-lua state.
 * - l the state to configure
 * - argv argument text strings to copy
 * - argc number of arguments
 * - script position of script name
 * @return one on success, zero otherwise
 */
int ledger_lua_set_arg
  (struct ledger_lua* l, char **argv, int argc, int script);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_LuaExt_H__*/
