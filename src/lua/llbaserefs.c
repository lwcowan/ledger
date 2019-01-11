
#include "llbaserefs.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../base/table.h"
#include <limits.h>
#include <string.h>


static char const* ledger_llbase_account_meta = "ledger.account";
static char const* ledger_llbase_entry_meta = "ledger.entry";

/*   BEGIN ledger/base/account { */

/*
 * `ledger.account.create()`
 * @return an account instance
 */
static int ledger_luaL_account_create(struct lua_State *L);

/*
 * `ledger.account.__gc(self~ledger.account)`
 * - self the account to free
 */
static int ledger_luaL_account___gc(struct lua_State *L);

/*
 * `ledger.account.__eq(a~ledger.account, b~ledger.account)`
 * - a first account
 * - b second account
 * @return true if equal, false otherwise
 */
static int ledger_luaL_account___eq(struct lua_State *L);

/*
 * `ledger.account.ptr(self~ledger.account)`
 * - self the account to query
 * @return the direct pointer to the account
 */
static int ledger_luaL_account_ptr(struct lua_State *L);

/*
 * `ledger.account.getdescription(self~ledger.account)`
 * - self the account to query
 * @return the account's description
 */
static int ledger_luaL_account_getdescription(struct lua_State *L);

/*
 * `ledger.account.setdescription(self~ledger.account, d~string)`
 * - self the account to modify
 * - d new description
 * @return a success flag
 */
static int ledger_luaL_account_setdescription(struct lua_State *L);

/*
 * `ledger.account.getname(self~ledger.account)`
 * - self the account to query
 * @return the account's name
 */
static int ledger_luaL_account_getname(struct lua_State *L);

/*
 * `ledger.account.setname(self~ledger.account, n~string)`
 * - self the account to modify
 * - n new name
 * @return a success flag
 */
static int ledger_luaL_account_setname(struct lua_State *L);

/*
 * `ledger.account.getid(self~ledger.account)`
 * - self the account to query
 * @return the account's identifier
 */
static int ledger_luaL_account_getid(struct lua_State *L);

/*
 * `ledger.account.setid(self~ledger.account, v~number)`
 * - self the account to modify
 * - v new identifier
 * @return a success flag
 */
static int ledger_luaL_account_setid(struct lua_State *L);

/*
 * `ledger.account.gettable(self~ledger.account)`
 * - self the account to query
 * @return the account's table
 */
static int ledger_luaL_account_gettable(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_account_lib[] = {
  {"create", ledger_luaL_account_create},
  {"__gc", ledger_luaL_account___gc},
  {"__eq", ledger_luaL_account___eq},
  {"ptr", ledger_luaL_account_ptr},
  {"getdescription", ledger_luaL_account_getdescription},
  {"setdescription", ledger_luaL_account_setdescription},
  {"getname", ledger_luaL_account_getname},
  {"setname", ledger_luaL_account_setname},
  {"getid", ledger_luaL_account_getid},
  {"setid", ledger_luaL_account_setid},
  {"gettable", ledger_luaL_account_gettable},
  {NULL, NULL}
};

/* } END   ledger/base/account */

/*   BEGIN ledger/base/entry { */

/*
 * `ledger.entry.create()`
 * @return an entry instance
 */
static int ledger_luaL_entry_create(struct lua_State *L);

/*
 * `ledger.entry.__gc(self~ledger.entry)`
 * - self the entry to free
 */
static int ledger_luaL_entry___gc(struct lua_State *L);

/*
 * `ledger.entry.__eq(a~ledger.entry, b~ledger.entry)`
 * - a first entry
 * - b second entry
 * @return true if equal, false otherwise
 */
static int ledger_luaL_entry___eq(struct lua_State *L);

/*
 * `ledger.entry.ptr(self~ledger.entry)`
 * - self the entry to query
 * @return the direct pointer to the entry
 */
static int ledger_luaL_entry_ptr(struct lua_State *L);

/*
 * `ledger.entry.getdescription(self~ledger.entry)`
 * - self the entry to query
 * @return the entry's description
 */
static int ledger_luaL_entry_getdescription(struct lua_State *L);

/*
 * `ledger.entry.setdescription(self~ledger.entry, d~string)`
 * - self the entry to modify
 * - d new description
 * @return a success flag
 */
static int ledger_luaL_entry_setdescription(struct lua_State *L);

/*
 * `ledger.entry.getname(self~ledger.entry)`
 * - self the entry to query
 * @return the entry's name
 */
static int ledger_luaL_entry_getname(struct lua_State *L);

/*
 * `ledger.entry.setname(self~ledger.entry, n~string)`
 * - self the entry to modify
 * - n new name
 * @return a success flag
 */
static int ledger_luaL_entry_setname(struct lua_State *L);

/*
 * `ledger.entry.getdate(self~ledger.entry)`
 * - self the entry to query
 * @return the entry's date
 */
static int ledger_luaL_entry_getdate(struct lua_State *L);

/*
 * `ledger.entry.setdate(self~ledger.entry, n~string)`
 * - self the entry to modify
 * - n new date
 * @return a success flag
 */
static int ledger_luaL_entry_setdate(struct lua_State *L);

/*
 * `ledger.entry.getid(self~ledger.entry)`
 * - self the entry to query
 * @return the entry's identifier
 */
static int ledger_luaL_entry_getid(struct lua_State *L);

/*
 * `ledger.entry.setid(self~ledger.entry, v~number)`
 * - self the entry to modify
 * - v new identifier
 * @return a success flag
 */
static int ledger_luaL_entry_setid(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_entry_lib[] = {
  {"create", ledger_luaL_entry_create},
  {"__gc", ledger_luaL_entry___gc},
  {"__eq", ledger_luaL_entry___eq},
  {"ptr", ledger_luaL_entry_ptr},
  {"getdescription", ledger_luaL_entry_getdescription},
  {"setdescription", ledger_luaL_entry_setdescription},
  {"getname", ledger_luaL_entry_getname},
  {"setname", ledger_luaL_entry_setname},
  {"getdate", ledger_luaL_entry_getdate},
  {"setdate", ledger_luaL_entry_setdate},
  {"getid", ledger_luaL_entry_getid},
  {"setid", ledger_luaL_entry_setid},
  {NULL, NULL}
};

/* } END   ledger/base/entry */


/* BEGIN static implementation */

/*   BEGIN ledger/base/account { */

int ledger_luaL_account_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return~ledger.account
   * THROW:
   *   X
   */
  struct ledger_account* next_account;
  int ok;
  /* execute C API */{
    next_account = ledger_account_new();
    if (next_account != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llbase_postaccount
    (L, next_account, ok, "ledger.account.create: generic error");
  return 1;
}

int ledger_luaL_account___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   * RET:
   *   X
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  ledger_account_free(*a);
  *a = NULL;
  return 0;
}

int ledger_luaL_account_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.account
   *   2  right~ledger.account
   * ...:
   */
  struct ledger_account** left =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  struct ledger_account** right =
    (struct ledger_account**)luaL_checkudata
        (L, 2, ledger_llbase_account_meta);
  if (*left != NULL && *right != NULL){
    return ledger_account_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_account_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_account___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.account
   *   2  right~ledger.account
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_account_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_account_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  lua_pushlightuserdata(L, (void*)*a);
  return 1;
}

int ledger_luaL_account_getdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   * RET:
   *   2  @return~string
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  unsigned char const* d = ledger_account_get_description(*a);
  lua_pushstring(L, (char const*)d);
  return 1;
}

int ledger_luaL_account_setdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   *   2  d~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  unsigned char const* d = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_account_set_description(*a, d);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_account_getname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   * RET:
   *   2  @return~string
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  unsigned char const* n = ledger_account_get_name(*a);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_account_setname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_account_set_name(*a, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_account_getid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   * RET:
   *   2  @return~number
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  int v = ledger_account_get_id(*a);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_account_setid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  int const v = (int)lua_tointeger(L, 2);
  ledger_account_set_id(*a, v);
  lua_pushboolean(L, 1);
  return 1;
}

int ledger_luaL_account_gettable(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.account
   * RET:
   *   2  @return~ledger.table
   */
  struct ledger_account** a =
    (struct ledger_account**)luaL_checkudata
        (L, 1, ledger_llbase_account_meta);
  struct ledger_table* t = ledger_account_get_table(*a);
  if (ledger_table_acquire(t) != t){
    luaL_error(L, "ledger.account.gettable: Table unavailable");
  } else {
    ledger_llbase_posttable
      (L, t, 1, "ledger.account.gettable: Table available");
  }
  return 1;
}

/* } END   ledger/base/account */

/*   BEGIN ledger/base/entry { */

int ledger_luaL_entry_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return~ledger.entry
   * THROW:
   *   X
   */
  struct ledger_entry* next_entry;
  int ok;
  /* execute C API */{
    next_entry = ledger_entry_new();
    if (next_entry != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llbase_postentry
    (L, next_entry, ok, "ledger.entry.create: generic error");
  return 1;
}

int ledger_luaL_entry___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   * RET:
   *   X
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  ledger_entry_free(*a);
  *a = NULL;
  return 0;
}

int ledger_luaL_entry_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.entry
   *   2  right~ledger.entry
   * ...:
   */
  struct ledger_entry** left =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  struct ledger_entry** right =
    (struct ledger_entry**)luaL_checkudata
        (L, 2, ledger_llbase_entry_meta);
  if (*left != NULL && *right != NULL){
    return ledger_entry_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_entry_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_entry___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.entry
   *   2  right~ledger.entry
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_entry_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_entry_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  lua_pushlightuserdata(L, (void*)*a);
  return 1;
}

int ledger_luaL_entry_getdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   * RET:
   *   2  @return~string
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  unsigned char const* d = ledger_entry_get_description(*a);
  lua_pushstring(L, (char const*)d);
  return 1;
}

int ledger_luaL_entry_setdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   *   2  d~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  unsigned char const* d = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_entry_set_description(*a, d);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_entry_getname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   * RET:
   *   2  @return~string
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  unsigned char const* n = ledger_entry_get_name(*a);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_entry_setname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_entry_set_name(*a, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_entry_getdate(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   * RET:
   *   2  @return~string
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  unsigned char const* n = ledger_entry_get_date(*a);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_entry_setdate(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_entry_set_date(*a, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_entry_getid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   * RET:
   *   2  @return~number
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  int v = ledger_entry_get_id(*a);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_entry_setid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.entry
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  struct ledger_entry** a =
    (struct ledger_entry**)luaL_checkudata
        (L, 1, ledger_llbase_entry_meta);
  int const v = (int)lua_tointeger(L, 2);
  ledger_entry_set_id(*a, v);
  lua_pushboolean(L, 1);
  return 1;
}

/* } END   ledger/base/entry */

/* END   static implementation */

/* BEGIN implementation */

void ledger_luaopen_baserefs(struct lua_State *L){
  /* add account lib */{
    luaL_newlib(L, ledger_luaL_account_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_account_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_account_meta);
    }
    lua_setfield(L, -2, "account");
  }
  /* add entry lib */{
    luaL_newlib(L, ledger_luaL_entry_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_entry_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_entry_meta);
    }
    lua_setfield(L, -2, "entry");
  }
  return;
}

/* END   implementation */
