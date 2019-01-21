
#include "llactutil.h"
#include "llact.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../act/path.h"
#include "../act/select.h"
#include "../base/book.h"
#include "../base/util.h"
#include <limits.h>
#include <string.h>
#include <ctype.h>


static char const* ledger_llact_path_meta = "ledger.path";
static char const* ledger_llbase_book_meta = "ledger.book";

/*   BEGIN ledger/act/path { */

/*
 * `ledger.path.__gc(s)`
 * - s path to collect
 */
static int ledger_luaL_path___gc(struct lua_State *L);

/*
 * `ledger.path.__eq(s, t)`
 * - s first path
 * - t second path
 */
static int ledger_luaL_path___eq(struct lua_State *L);

/*
 * `ledger.path.root(k)`
 * - k book at which to start
 * @return the root path
 */
static int ledger_luaL_path_root(struct lua_State *L);

/*
 * `ledger.path.__tostring(s)`
 * - s path to convert to a string
 * @return a string version of the path
 */
static int ledger_luaL_path_tostring(struct lua_State *L);

/*
 * `ledger.path.create(x, s)`
 * - x either base path or book
 * - s path string to convert to an index path
 * @return the path on success
 */
static int ledger_luaL_path_create(struct lua_State *L);

/* [INTERNAL]
 * Convert a comparison string to a comparator code.
 * - str the string to convert
 * @return a corresponding code, or -1 if the string is invalid
 */
static int ledger_luaL_select_cond_atoi(char const* str);

static const struct luaL_Reg ledger_luaL_path_lib[] = {
  {"__gc", ledger_luaL_path___gc},
  {"create", ledger_luaL_path_create},
  {"__tostring", ledger_luaL_path_tostring},
  {"tostring", ledger_luaL_path_tostring},
  {"root", ledger_luaL_path_root},
  {"__eq", ledger_luaL_path___eq},
  {NULL,NULL}
};

/* } END   ledger/act/path */

/*   BEGIN ledger/act/select { */

/*
 * `ledger.select.cond(cmp~string|~number, column~number, value~string)`
 * - cmp comparison operator
 * - column column number
 * - value string representation of value against which to compare
 * @return the table on success
 */
static int ledger_luaL_select_cond(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_select_lib[] = {
  {"cond", ledger_luaL_select_cond},
  {NULL,NULL}
};

struct ledger_luaL_select_cmp {
  int code;
  char const* text;
};

struct ledger_luaL_select_cmp ledger_luaL_select_cmps[] = {
  { LEDGER_SELECT_EQUAL, "==" },
  { LEDGER_SELECT_LESS, "<" },
  { LEDGER_SELECT_MORE, ">" },
  { LEDGER_SELECT_NOTEQUAL, "~="},
  { LEDGER_SELECT_NOTLESS, ">=" },
  { LEDGER_SELECT_NOTMORE, "<=" },
  { LEDGER_SELECT_ID, "id" },
  { LEDGER_SELECT_BIGNUM, "bignum" },
  { LEDGER_SELECT_STRING, "string" }
};

/* } END   ledger/act/select */

/* BEGIN static implementation */

/*   BEGIN ledger/act/path { */

int ledger_luaL_path___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.path
   * RET:
   *   X
   * THROW:
   *   X
   */
  return 0;
}

int ledger_luaL_path_root(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2 @return~ledger.path
   * THROW:
   *   X
   */
  struct ledger_act_path path;
  struct ledger_book** j;
  struct ledger_book* acquired_j;
  j = (struct ledger_book**)luaL_checkudata
      (L, -1, ledger_llbase_book_meta);
  acquired_j = ledger_book_acquire(*j);
  if (acquired_j != *j){
    luaL_error(L,
      "ledger.path.root: could not acquire source book");
  }
  path = ledger_act_path_root();
  ledger_llact_postpath(L, path, acquired_j, 1, "ledger.path.root: success");
  return 1;
}

int ledger_luaL_path___eq(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.path
   *   2  ~ledger.path
   * RET:
   *   3 @return~boolean
   * THROW:
   *   X
   */
  int result = 1;
  struct ledger_act_path path1, path2;
  struct ledger_book* book1, * book2;
  ledger_llact_getpath
      (L, &path1, &book1, 0, "ledger.path.__eq: failed to process path");
  lua_rotate(L, 1, 1);
  ledger_llact_getpath
      (L, &path2, &book2, 0, "ledger.path.__eq: failed to process path");
  do {
    int i;
    if (book1 != book2){
      result = 0;
      break;
    }
    if (path1.typ != path2.typ){
      result = 0;
      break;
    }
    if (path1.len != path2.len){
      result = 0;
      break;
    }
    for (i = 0; i < path1.len; ++i){
      if (path1.path[i] != path2.path[i])
        break;
      else
        continue;
    }
    if (i < path1.len){
      result = 0;
      break;
    }
  } while (0);
  ledger_book_free(book1);
  ledger_book_free(book2);
  lua_pushboolean(L, result!=0);
  return 1;
}

int ledger_luaL_path_tostring(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.path
   * RET:
   *   2 @return~string
   * THROW:
   *   X
   */
  int ok;
  int text_length;
  unsigned char *outstr;
  struct ledger_act_path path;
  struct ledger_book* book;
  ledger_llact_getpath
      (L, &path, &book, 1, "ledger.path.__tostring: failed to process path");
  /* execute C API */{
    text_length = ledger_act_path_render(NULL, 0, path, book);
    if (text_length > 0){
      size_t maxlen = text_length;
      outstr = ledger_util_malloc(maxlen+1);
      if (outstr == NULL){
        ok = 0;
      } else {
        ledger_act_path_render(outstr, text_length+1, path, book);
        outstr[maxlen] = 0;
        ok = 1;
      }
    } else {
      outstr = NULL;
      ok = 0;
    }
  }
  ledger_book_free(book);
  ledger_llbase_poststring
    (L, outstr, ok, "ledger.path.__tostring: generic error");
  return 1;
}

int ledger_luaL_path_create(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.path|~ledger.book
   *   2  ~string
   * RET:
   *   3 @return~ledger.path
   * THROW:
   *   X
   */
  int ok;
  struct ledger_act_path path;
  struct ledger_act_path next_path;
  struct ledger_book* book;
  struct ledger_book** j;
  unsigned char const *instr;
  instr = (unsigned char const*)luaL_checkstring(L, 2);
  j = luaL_testudata(L, 1, ledger_llbase_book_meta);
  if (j != NULL){
    book = ledger_book_acquire(*j);
    if (book != *j){
      luaL_error(L,
        "ledger.path.create: could not acquire source book");
    }
    path = ledger_act_path_root();
  } else {
    ledger_llact_getpath
      (L, &path, &book, 1, "ledger.path.create: failed to process base path");
  }
  /* execute C API */{
    next_path = ledger_act_path_compute(book, instr, path, &ok);
    if (!ok){
      ledger_book_free(book);
      luaL_error(L,
        "ledger.path.create: path not valid");
    }
  }
  ledger_llact_postpath(L, next_path, book, ok,
      "ledger.path.create: path not valid");
  return 1;
}

/* } END   ledger/act/path */

/*   BEGIN ledger/act/select { */

int ledger_luaL_select_cond(struct lua_State *L){
  /* ARG:
   *   1  cmp~number|~string
   *   2  column~number
   *   3  value~string
   * RET:
   *   4 @return~table
   * THROW:
   *   X
   */
  lua_Integer column_number;
  lua_Integer comparator;
  lua_createtable(L, 0, 3);
  /* set the "value" field */{
    (void)luaL_tolstring(L, 3, NULL);
    lua_setfield(L, -2, "value");
  }
  /* set the "column" field */{
    column_number = lua_tointeger(L, 2);
    lua_pushinteger(L, column_number);
    lua_setfield(L, -2, "column");
  }
  /* set the comparator field */{
    if (lua_isnumber(L, 1)){
      comparator = lua_tointeger(L, 1);
    } else {
      char const* comparison_text;
      comparison_text = luaL_tolstring(L, 1, NULL);
      comparator = ledger_luaL_select_cond_atoi(comparison_text);
      lua_pop(L, 1);
    }
    lua_pushinteger(L, comparator);
    lua_setfield(L, -2, "cmp");
  }
  return 1;
}

int ledger_luaL_select_cond_atoi(char const* str){
  int code = 0;
  char const* p;
  for (p = str; *p != 0; ++p){
    /* find first non-space */
    if (isspace(*p)){
      continue;
    } else {
      /* find next non-space */
      char const* q;
      ptrdiff_t token_len;
      int i;
      int const len =
        sizeof(ledger_luaL_select_cmps)/sizeof(*ledger_luaL_select_cmps);
      for (q = p+1; *q != 0; ++q){
        if (isspace(*q)) break;
      }
      token_len = q-p;
      /* iterate over comparison strings */
      for (i = 0; i < len; ++i){
        if (strncmp(p, ledger_luaL_select_cmps[i].text, token_len) != 0){
          continue;
        }
        if (strlen(ledger_luaL_select_cmps[i].text) == token_len){
          break;
        }
      }
      if (i < len){
        code |= ledger_luaL_select_cmps[i].code;
        p = q;
      } else break;
    }
  }
  if (*p == 0)
    return code;
  else
    return -1;
}

/* } END   ledger/act/select */

/* END   static implementation */

/* BEGIN implementation */

void ledger_luaopen_actutil(struct lua_State *L){
  /* add path lib */{
    luaL_newlib(L, ledger_luaL_path_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llact_path_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llact_path_meta);
    }
    lua_setfield(L, -2, "path");
  }
  /* add select lib */{
    luaL_newlib(L, ledger_luaL_select_lib);
    lua_setfield(L, -2, "select");
  }
  return;
}

/* END   implementation */
