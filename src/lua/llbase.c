
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../base/util.h"
#include <limits.h>


/* [INTERNAL]
 * Post the string on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_poststring(struct lua_State *L);

/*   BEGIN ledger/base/util { */

/*
 * `ledger.util.ustrdup(s)`
 * - s string to duplicate
 * @return the duplicated string
 */
static int ledger_luaL_util_ustrdup(struct lua_State *L);

/*
 * `ledger.util.ustrndup(s, len)`
 * - s string to duplicate
 * - len number of bytes to duplicate
 * @return the duplicated string
 */
static int ledger_luaL_util_ustrndup(struct lua_State *L);

/*
 * `ledger.util.ustrlen(s)`
 * - s string to analyze
 * @return the length of the corresponding C-string
 */
static int ledger_luaL_util_ustrlen(struct lua_State *L);

/*
 * `ledger.util.ustrcmp(s1,s2)`
 * - s1 first string to compare
 * - s2 second string to compare
 * @return negative if s1<s2, zero if s1==s2, positive if s1>s2
 */
static int ledger_luaL_util_ustrcmp(struct lua_State *L);

/*
 * `ledger.util.ustrcmp(s1,s2,n)`
 * - s1 first string to compare
 * - s2 second string to compare
 * - n max number of bytes to compare
 * @return negative if s1<s2, zero if s1==s2, positive if s1>s2
 */
static int ledger_luaL_util_ustrncmp(struct lua_State *L);

/*
 * `ledger.util.uiszero(s[,n])`
 * - s string to analyze
 * - n number of bytes to check
 * @return whether the string is full of zeroes
 */
static int ledger_luaL_util_uiszero(struct lua_State *L);

/*
 * `ledger.util.itoa(n[, want_plus])
 * - n number to convert
 * - want_plus whether the user wants extra plus signs
 * @return a string representation of the integer
 */
static int ledger_luaL_util_itoa(struct lua_State *L);

/*
 * `ledger.util.atoi(s)
 * - s string to convert
 * @return an integer from the string
 */
static int ledger_luaL_util_atoi(struct lua_State *L);

/* } END   ledger/base/util */

static const struct luaL_Reg ledger_luaL_util_lib[] = {
  {"ustrdup", ledger_luaL_util_ustrdup},
  {"ustrndup", ledger_luaL_util_ustrndup},
  {"ustrlen", ledger_luaL_util_ustrlen},
  {"ustrcmp", ledger_luaL_util_ustrcmp},
  {"ustrncmp", ledger_luaL_util_ustrncmp},
  {"uiszero", ledger_luaL_util_uiszero},
  {"itoa", ledger_luaL_util_itoa},
  {"atoi", ledger_luaL_util_atoi},
  {NULL,NULL}
};

/* BEGIN static implementation */

int ledger_llbase_poststring(struct lua_State *L){
  /* ARG:
   *   1 *str
   * RET:
   *   2 @return
   */
  unsigned char const* str = (unsigned char const*)lua_touserdata(L, 1);
  lua_pushstring(L, (char const*)str);
  return 1;
}

/*   BEGIN ledger/base/util { */
int ledger_luaL_util_ustrdup(struct lua_State *L){
  /* ARG:
   *   1  str
   * RET:
   *   2 @return
   * THROW:
   *   X
   */
  char const* str = lua_tostring (L, 1);
  /* implicit clone */
  lua_pushstring(L, str);
  return 1;
}

int ledger_luaL_util_ustrndup(struct lua_State *L){
  /* ARG:
   *   1  str
   *   2  sz
   * RET:
   *   3 @return
   * THROW:
   *   X
   */
  size_t maxlen;
  unsigned char *outstr;
  unsigned char const* str =
    (unsigned char const*)lua_tolstring(L, 1, &maxlen);
  lua_Integer sz = lua_tointeger(L, 2);
  int ok;
  /* execute C API */{
    if (sz < 0 || sz > INT_MAX){
      outstr = NULL;
      ok = 0;
    } else {
      if (sz > maxlen){
        sz = maxlen;
      }
      outstr = ledger_util_ustrndup(str, (size_t)sz, &ok);
    }
  }
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_poststring);
    lua_pushlightuserdata(L, outstr);
    success_line = lua_pcall(L, 1, 1, 0);
    ledger_util_free(outstr);
    if (success_line != LUA_OK){
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    luaL_error(L, "ledger.util.strndup: generic error");
  }
  return 1;
}

int ledger_luaL_util_ustrlen(struct lua_State *L){
  luaL_checktype(L, 1, LUA_TSTRING);
  /* execute */{
    unsigned char const* str =
      (unsigned char const*)lua_tostring(L, 1);
    size_t sz = ledger_util_ustrlen(str);
    lua_pushinteger(L, (lua_Integer)sz);
  }
  return 1;
}

int ledger_luaL_util_ustrcmp(struct lua_State *L){
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  /* execute */{
    unsigned char const* str1 =
      (unsigned char const*)lua_tostring(L, 1);
    unsigned char const* str2 =
      (unsigned char const*)lua_tostring(L, 2);
    int cmp_value = ledger_util_ustrcmp(str1, str2);
    lua_pushinteger(L, (lua_Integer)cmp_value);
  }
  return 1;
}

int ledger_luaL_util_ustrncmp(struct lua_State *L){
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);
  /* execute */{
    unsigned char const* str1 =
      (unsigned char const*)lua_tostring(L, 1);
    unsigned char const* str2 =
      (unsigned char const*)lua_tostring(L, 2);
    lua_Integer len = lua_tointeger(L, 3);
    int cmp_value = ledger_util_ustrncmp(str1, str2, len);
    lua_pushinteger(L, (lua_Integer)cmp_value);
  }
  return 1;
}

int ledger_luaL_util_uiszero(struct lua_State *L){
  lua_Integer sz;
  luaL_checktype(L, 1, LUA_TSTRING);
  if (lua_isnumber(L, 2)){
    sz = lua_tointeger(L, 2);
  } else sz = -1;
  /* execute */{
    size_t maxlen;
    unsigned char const* str1 =
      (unsigned char const*)lua_tolstring(L, 1, &maxlen);
    if (sz < 0 || sz > maxlen) sz = maxlen;
    int result = ledger_util_uiszero(str1, sz);
    lua_pushboolean(L, result);
  }
  return 1;
}

int ledger_luaL_util_itoa(struct lua_State *L){
  /* ARG:
   *   1  n
   * RET:
   *   2 @return
   * THROW:
   *   X
   */
  unsigned char *outstr;
  lua_Integer n;
  int want_plus;
  int ok;
  luaL_checktype(L, 1, LUA_TNUMBER);
  n = lua_tointeger(L, 1);
  want_plus = lua_toboolean(L, 2);
  /* execute C API */{
    size_t maxlen = ledger_util_itoa((int)n, NULL, 0, want_plus);
    outstr = ledger_util_malloc(maxlen+1);
    if (outstr == NULL){
      ok = 0;
    } else {
      ledger_util_itoa((int)n, outstr, maxlen+1, want_plus);
      outstr[maxlen] = 0;
      ok = 1;
    }
  }
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_poststring);
    lua_pushlightuserdata(L, outstr);
    success_line = lua_pcall(L, 1, 1, 0);
    ledger_util_free(outstr);
    if (success_line != LUA_OK){
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    luaL_error(L, "ledger.util.itoa: generic error");
  }
  return 1;
}

int ledger_luaL_util_atoi(struct lua_State *L){
  luaL_checktype(L, 1, LUA_TSTRING);
  /* execute */{
    unsigned char const* str1 =
      (unsigned char const*)lua_tostring(L, 1);
    int result = ledger_util_atoi(str1);
    lua_pushinteger(L, result);
  }
  return 1;
}

/* } END   ledger/base/util */

/* END   static implementation */

/* BEGIN implementation */

void ledger_luaopen_base(struct lua_State *L){
  /* fetch the global `ledger` table */{
    int type = lua_getglobal (L, "ledger");
    if (type != LUA_TTABLE){
      luaL_error(L, "ledger_luaopen_base: bad `ledger` global");
      return;
    }
  }
  /* add util lib */{
    luaL_newlib(L, ledger_luaL_util_lib);
    lua_setfield(L, -2, "util");
  }
  return;
}

/* END   implementation */
