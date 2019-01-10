
#include "llbaseutil.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../base/sum.h"
#include <limits.h>


static char const* ledger_llbase_bignum_meta = "ledger.bignum";
static char const* ledger_llbase_table_meta = "ledger.table";

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

/* } END   ledger/base/util */

/*   BEGIN ledger/base/bignum { */

/*
 * `ledger.bignum.create([v~string | v~number [, digits, point]])`
 * - v the value to convert to a big number
 * - digits minimum number of centesimal digits for this number
 * - point position of the centesimal point
 * @return a big number instance
 */
static int ledger_luaL_bignum_create(struct lua_State *L);

/*
 * `ledger.bignum.__gc(self~ledger.bignum)`
 * - self the number to free
 */
static int ledger_luaL_bignum___gc(struct lua_State *L);

/*
 * `ledger.bignum.__eq(a~ledger.bignum, b~ledger.bignum)`
 * - a first number
 * - b second number
 * @return true if equal, false otherwise
 */
static int ledger_luaL_bignum___eq(struct lua_State *L);

/*
 * `ledger.bignum.__lt(a~ledger.bignum, b~ledger.bignum)`
 * - a first number
 * - b second number
 * @return true if `a` is less than `b`, false otherwise
 */
static int ledger_luaL_bignum___lt(struct lua_State *L);

/*
 * `ledger.bignum.__le(a~ledger.bignum, b~ledger.bignum)`
 * - a first number
 * - b second number
 * @return true if `a` is less or equal to `b`, false otherwise
 */
static int ledger_luaL_bignum___le(struct lua_State *L);

/*
 * `ledger.bignum.tostring(self~ledger.bignum)`
 * `ledger.bignum.__tostring(self~ledger.bignum)`
 * - self the number to query
 * @return a string representation of the big number
 */
static int ledger_luaL_bignum_tostring(struct lua_State *L);

/*
 * `ledger.bignum.tointeger(self~ledger.bignum)`
 * - self the number to query
 * @return an integer representation of the big number
 */
static int ledger_luaL_bignum_tointeger(struct lua_State *L);

/*
 * `ledger.bignum.__len(self~ledger.bignum)`
 * - self the number to query
 * @return the number of centesimal digits in the number
 */
static int ledger_luaL_bignum___len(struct lua_State *L);

/*
 * `ledger.bignum.findpoint(self~ledger.bignum)`
 * - self the number to query
 * @return the position of the centesimal point
 */
static int ledger_luaL_bignum_findpoint(struct lua_State *L);

/*
 * `ledger.bignum.__unm(self~ledger.bignum)`
 * - self the number to negate
 * @return a negated version of this number
 */
static int ledger_luaL_bignum___unm(struct lua_State *L);

/*
 * `ledger.bignum.__add(left~ledger.bignum, right~ledger.bignum)`
 * - left one addend
 * - right another addend
 * @return the sum of the two big numbers
 */
static int ledger_luaL_bignum___add(struct lua_State *L);

/*
 * `ledger.bignum.__sub(left~ledger.bignum, right~ledger.bignum)`
 * - left the minuend
 * - right the subtrahend
 * @return the sum of the two big numbers
 */
static int ledger_luaL_bignum___sub(struct lua_State *L);

/* [INTERNAL]
 * Compare two big numbers at positions #1 and #2 on the
 *   Lua stack.
 * - L Lua state to interrogate
 * @return zero if equal, negative if #1 is less that #2,
 *   positive if #1 is greater than #2
 */
static int ledger_luaL_bignum_cmp(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_bignum_lib[] = {
  {"create", ledger_luaL_bignum_create},
  {"__gc", ledger_luaL_bignum___gc},
  {"__eq", ledger_luaL_bignum___eq},
  {"__lt", ledger_luaL_bignum___lt},
  {"__le", ledger_luaL_bignum___le},
  {"__tostring", ledger_luaL_bignum_tostring},
  {"tostring", ledger_luaL_bignum_tostring},
  {"tointeger", ledger_luaL_bignum_tointeger},
  {"__len", ledger_luaL_bignum___len},
  {"findpoint", ledger_luaL_bignum_findpoint},
  {"__unm", ledger_luaL_bignum___unm},
  {"__add", ledger_luaL_bignum___add},
  {"__sub", ledger_luaL_bignum___sub},
  {NULL,NULL}
};

/* } END   ledger/base/util */

/*   BEGIN ledger/base/sum { */

/* [INTERNAL]
 * Perform a column sum.
 * - L Lua state
 * - arg argument offset
 * @return one on success
 */
static int ledger_luaL_sum_sumC1(struct lua_State *L, int arg);

/* [INTERNAL]
 * `ledger.sum.sum(~ledger.table, ~number)`
 * - L Lua state
 * @return one on success
 */
static int ledger_luaL_sum_sum(struct lua_State *L);

/* [INTERNAL]
 * `ledger.sum(~ledger.table, ~number)`
 * - L Lua state
 * @return one on success
 */
static int ledger_luaL_sum___call(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_sum_lib[] = {
  {"sum", ledger_luaL_sum_sum},
  {NULL,NULL}
};

static const struct luaL_Reg ledger_luaL_sum_metalib[] = {
  {"__call", ledger_luaL_sum___call},
  {NULL,NULL}
};

/* } END   ledger/base/sum */

/* BEGIN static implementation */

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
  ledger_llbase_poststring
    (L, outstr, ok, "ledger.util.ustrndup: generic error");
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
   *   2 ?want_plus
   * RET:
   *   3 @return
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
  ledger_llbase_poststring
    (L, outstr, ok, "ledger.util.itoa: generic error");
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

/*   BEGIN ledger/base/bignum { */

int ledger_luaL_bignum_create(struct lua_State *L){
  /* ARG:
   *   1 ?value~(string|number)
   *   2 ?digits~number
   *   2 ?point~number
   * RET:
   *   3 @return
   * THROW:
   *   X
   */
  unsigned char const* value_str = NULL;
  lua_Integer value_int = 0;
  int value_type;
  struct ledger_bignum* next_bignum;
  int digits = 0;
  int point = 0;
  int ok;
  if (lua_isinteger(L, 1)){
    value_type = LUA_NUMTAGS+1;
    value_int = lua_tointeger(L, 1);
  } else if (lua_isstring(L, 1)){
    value_type = LUA_TSTRING;
    value_str = (unsigned char const*)lua_tostring(L, 1);
  } else if (lua_isnoneornil(L, 1)){
    value_type = LUA_TNIL;
  } else {
    luaL_error(L, "ledger.bignum.create: illegal argument #1");
    return 0;
  }
  if (value_type != LUA_TNIL){
    /* process other arguments */
    digits = (int)luaL_optinteger(L, 2, 0);
    point = (int)luaL_optinteger(L, 3, 0);
  }
  /* execute C API */{
    next_bignum = ledger_bignum_new();
    if (next_bignum != NULL){
      int sub_ok;
      if (digits > 0){
        sub_ok = ledger_bignum_alloc(next_bignum, digits, point);
      } else sub_ok = 1;
      if (sub_ok) switch (value_type){
      case LUA_NUMTAGS+1:
        {
          ok = ledger_bignum_set_long(next_bignum, (long)value_int);
        }break;
      case LUA_TSTRING:
        {
          ok = ledger_bignum_set_text(next_bignum, value_str, NULL);
        }break;
      default:
        ok = 1;
        break;
      }
    } else ok = 0;
  }
  ledger_llbase_postbignum
    (L, next_bignum, ok, "ledger.bignum.create: generic error");
  return 1;
}

int ledger_luaL_bignum___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.bignum
   * RET:
   *   X
   */
  struct ledger_bignum** num =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  ledger_bignum_free(*num);
  *num = NULL;
  return 0;
}

int ledger_luaL_bignum_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.bignum
   *   2  right~ledger.bignum
   * ...:
   */
  struct ledger_bignum** left =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  struct ledger_bignum** right =
    (struct ledger_bignum**)luaL_checkudata(L, 2, ledger_llbase_bignum_meta);
  if (*left != NULL && *right != NULL){
    return ledger_bignum_compare(*left, *right);
  } else {
    luaL_error(L, "ledger_luaL_bignum_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_bignum___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.bignum
   *   2  right~ledger.bignum
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_bignum_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_bignum___lt(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.bignum
   *   2  right~ledger.bignum
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_bignum_cmp(L);
  lua_pushboolean(L, result<0);
  return 1;
}

int ledger_luaL_bignum___le(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.bignum
   *   2  right~ledger.bignum
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_bignum_cmp(L);
  lua_pushboolean(L, result<=0);
  return 1;
}

int ledger_luaL_bignum_tostring(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.bignum
   *   2 ?want_plus
   * RET:
   *   X
   */
  struct ledger_bignum** num =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  unsigned char *outstr;
  int want_plus;
  int ok;
  want_plus = lua_toboolean(L, 2);
  /* execute C API */{
    size_t maxlen = ledger_bignum_get_text(*num, NULL, 0, want_plus);
    outstr = ledger_util_malloc(maxlen+1);
    if (outstr == NULL){
      ok = 0;
    } else {
      ledger_bignum_get_text(*num, outstr, maxlen+1, want_plus);
      outstr[maxlen] = 0;
      ok = 1;
    }
  }
  ledger_llbase_poststring
    (L, outstr, ok, "ledger.bignum.tostring: generic error");
  return 1;
}

int ledger_luaL_bignum_tointeger(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.bignum
   * RET:
   *   2  @return~integer
   */
  struct ledger_bignum** num =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  long out_value = ledger_bignum_get_long(*num);
  lua_pushinteger(L, (lua_Integer)out_value);
  return 1;
}

int ledger_luaL_bignum___len(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.bignum
   * RET:
   *   2  @return~integer
   */
  struct ledger_bignum** num =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  int out_value = ledger_bignum_count_digits(*num);
  lua_pushinteger(L, (lua_Integer)out_value);
  return 1;
}

int ledger_luaL_bignum_findpoint(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.bignum
   * RET:
   *   2  @return~integer
   */
  struct ledger_bignum** num =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  int out_value = ledger_bignum_find_point(*num);
  lua_pushinteger(L, (lua_Integer)out_value);
  return 1;
}

int ledger_luaL_bignum___unm(struct lua_State *L){
  /* ARG:
   *   1 value~ledger.bignum
   * RET:
   *   2 @return~ledger.bignum
   * THROW:
   *   X
   */
  struct ledger_bignum** num =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  struct ledger_bignum* next_bignum;
  int ok;
  /* execute C API */{
    next_bignum = ledger_bignum_new();
    if (next_bignum != NULL){
      ok = ledger_bignum_negate(next_bignum, *num);
    } else ok = 0;
  }
  ledger_llbase_postbignum
    (L, next_bignum, ok, "ledger.bignum.__unm: generic error");
  return 1;
}

int ledger_luaL_bignum___add(struct lua_State *L){
  /* ARG:
   *   1 left~ledger.bignum
   *   2 right~ledger.bignum
   * RET:
   *   3 @return~ledger.bignum
   * THROW:
   *   X
   */
  struct ledger_bignum** left =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  struct ledger_bignum** right =
    (struct ledger_bignum**)luaL_checkudata(L, 2, ledger_llbase_bignum_meta);
  struct ledger_bignum* next_bignum;
  int ok;
  /* execute C API */{
    next_bignum = ledger_bignum_new();
    if (next_bignum != NULL){
      ok = ledger_bignum_add(next_bignum, *left, *right);
    } else ok = 0;
  }
  ledger_llbase_postbignum
    (L, next_bignum, ok, "ledger.bignum.__add: generic error");
  return 1;
}

int ledger_luaL_bignum___sub(struct lua_State *L){
  /* ARG:
   *   1 left~ledger.bignum
   *   2 right~ledger.bignum
   * RET:
   *   3 @return~ledger.bignum
   * THROW:
   *   X
   */
  struct ledger_bignum** left =
    (struct ledger_bignum**)luaL_checkudata(L, 1, ledger_llbase_bignum_meta);
  struct ledger_bignum** right =
    (struct ledger_bignum**)luaL_checkudata(L, 2, ledger_llbase_bignum_meta);
  struct ledger_bignum* next_bignum;
  int ok;
  /* execute C API */{
    next_bignum = ledger_bignum_new();
    if (next_bignum != NULL){
      ok = ledger_bignum_subtract(next_bignum, *left, *right);
    } else ok = 0;
  }
  ledger_llbase_postbignum
    (L, next_bignum, ok, "ledger.bignum.__sub: generic error");
  return 1;
}

/* } END   ledger/base/bignum */

/*   BEGIN ledger/base/sum { */

int ledger_luaL_sum_sum(struct lua_State *L){
  /* ARG:
   *   1 table~ledger.table
   *   2 column~number
   * RET:
   *   3 @return~ledger.bignum
   * THROW:
   *   X
   */
  return ledger_luaL_sum_sumC1(L,0);
}

int ledger_luaL_sum___call(struct lua_State *L){
  /* ARG:
   *   1 ~table
   *   2 table~ledger.table
   *   3 column~number
   * RET:
   *   4 @return~ledger.bignum
   * THROW:
   *   X
   */
  return ledger_luaL_sum_sumC1(L,1);
}

int ledger_luaL_sum_sumC1(struct lua_State *L, int arg){
  int result;
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata
        (L, 1+arg, ledger_llbase_table_meta);
  int const column = lua_tointeger(L, 2+arg);
  struct ledger_bignum* bn;
  bn = ledger_bignum_new();
  if (bn == NULL){
    luaL_error(L, "ledger_luaL_sum_sumC1: "
      "Big number memory not available");
    /* return 0; */
  }
  result = ledger_sum_table_column(bn, *t, column);
  ledger_llbase_postbignum
    (L, bn, result, "ledger_luaL_sum_sumC1: "
        "Big number not available");
  return 1;
}

/* } END   ledger/base/sum */

/* END   static implementation */

/* BEGIN implementation */

void ledger_luaopen_baseutil(struct lua_State *L){
  /* add util lib */{
    luaL_newlib(L, ledger_luaL_util_lib);
    lua_setfield(L, -2, "util");
  }
  /* add bignum lib */{
    luaL_newlib(L, ledger_luaL_bignum_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_bignum_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_bignum_meta);
    }
    lua_setfield(L, -2, "bignum");
  }
  /* add sum lib */{
    luaL_newlib(L, ledger_luaL_sum_lib);
    luaL_newlib(L, ledger_luaL_sum_metalib);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, "sum");
  }
  return;
}

/* END   implementation */
