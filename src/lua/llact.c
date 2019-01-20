
#include "llact.h"
#include "llbase.h"
#include "llactutil.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../act/path.h"
#include "../base/book.h"
#include <limits.h>
#include <string.h>


/* [INTERNAL]
 * Post the path on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llact_postpathP1(struct lua_State *L);

/* [INTERNAL]
 * Get the path from on the top of the stack.
 * - L lua state to modify
 * @return zero
 */
static int ledger_llact_getpathP1(struct lua_State *L);

static char const* ledger_llact_path_meta = "ledger.path";
static char const* ledger_llbase_book_meta = "ledger.book";

/* BEGIN static implementation */

int ledger_llact_postpathP1(struct lua_State *L){
  /* ARG:
   *   1 *path
   *   2 book~ledger.book
   * RET:
   *   3 @return~ledger.path
   */
  /* Table structure:
   * [1] <- path[0] if len > 0
   * [2] <- path[1] if len > 1
   * ["type"] <- typ
   * ["source"] <- arg:book
   */
  int len;
  struct ledger_act_path const path =
    *(struct ledger_act_path*)lua_touserdata(L, 1);
  /* create the table */{
    if (path.len > 2) len = 2;
    else if (path.len < 0) len = 0;
    else len = path.len;
    lua_createtable(L, len, 2);
  }
  /* fill the table */{
    int i;
    for (i = 0; i < len; ++i){
      lua_pushinteger(L, path.path[i]);
      lua_seti(L, -2, i+1);
    }
  }
  /* set the type */{
    lua_pushinteger(L, path.typ);
    lua_setfield(L, -2, "type");
  }
  /* set the source */{
    lua_pushvalue(L, 2);
    lua_setfield(L, -2, "source");
  }
  /* apply the userdata registry item `ledger.path` to this userdata */
  luaL_setmetatable(L, ledger_llact_path_meta);
  return 1;
}

int ledger_llact_getpathP1(struct lua_State *L){
  /* ARG:
   *   1 ~table
   *   2 *path
   *   3 *book
   * RET:
   *   X
   */
  struct ledger_act_path *path =
    (struct ledger_act_path*)lua_touserdata(L, 2);
  struct ledger_book **book =
    (struct ledger_book**)lua_touserdata(L, 3);
  path->len = 0;
  path->path[0] = -1;
  path->path[1] = -1;
  path->typ = 0;
  *book = NULL;
  /* extract the index components */{
    lua_Integer len, i;
    lua_len(L, 1);
    len = lua_tointeger(L, -1);
    lua_pop(L, 1);
    if (len > 2) len = 2;
    else if (len < 0) len = 0;
    path->len = (int)len;
    for (i = 0; i < len; ++i){
      lua_geti(L, 1, i+1);
      path->path[i] = (int)lua_tointeger(L, -1);
      lua_pop(L, 1);
    }
  }
  /* extract the type */{
    lua_getfield(L, 1, "type");
    path->typ = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  /* extract the book */{
    struct ledger_book** j;
    lua_getfield(L, 1, "source");
    j = (struct ledger_book**)luaL_checkudata
        (L, -1, ledger_llbase_book_meta);
    if (*j == NULL){
      *book = NULL;
      luaL_error(L,
          "ledger_llact_getpath: expected book in source field, got nil");
    } else {
      struct ledger_book* acquired_j;
      acquired_j = ledger_book_acquire(*j);
      if (acquired_j != *j){
        *book = NULL;
        luaL_error(L,
          "ledger_llact_getpath: could not acquire source book");
      } else {
        *book = acquired_j;
      }
    }
  }
  return 0;
}

/* END static implementation */

/* BEGIN implementation */

int ledger_llact_postpath
  ( struct lua_State *L, struct ledger_act_path path,
    struct ledger_book* book, int ok, char const* err)
{
  if (!lua_checkstack(L, 3)){
    ledger_book_free(book);
    return 0;
  }
  if (ok){
    /* first make the book object */{
      int result;
      result = ledger_llbase_postbook
        (L, book, 1, "ledger_llact_postpath: attempt to make book available");
      if (!result){
        /* give up early */
        return 0;
      }
    }
    /* then make the path table */{
      int success_line;
      lua_pushcfunction(L, ledger_llact_postpathP1);
      lua_pushlightuserdata(L, &path);
      lua_rotate(L, lua_absindex(L, -3), -1);
      success_line = lua_pcall(L, 2, 1, 0);
      if (success_line != LUA_OK){
        ledger_book_free(book);
        /* throw; */
        lua_error(L);
      } else /* leave return at top of stack */;
    }
  } else {
    ledger_book_free(book);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llact_getpath
  ( struct lua_State *L, struct ledger_act_path *path,
    struct ledger_book** book, int throwing, char const* err)
{
  int success_line;
  struct ledger_act_path tmp_path;
  struct ledger_book* tmp_book;
  if ((!throwing) && (!lua_checkstack(L, 4))){
    return 0;
  }
  lua_pushcfunction(L, ledger_llact_getpathP1);
  lua_pushvalue(L, -2);
  lua_pushlightuserdata(L, &tmp_path);
  lua_pushlightuserdata(L, &tmp_book);
  success_line = lua_pcall(L, 3, 0, 0);
  if (success_line != LUA_OK){
    /* throw; */if (throwing){
      if (tmp_book != NULL){
        ledger_book_free(tmp_book);
      }
      lua_error(L);
    }
    *book = NULL;
    /* set to null path */{
      tmp_path.len = 0;
      tmp_path.path[0] = -1;
      tmp_path.path[1] = -1;
      tmp_path.typ = 0;
      memcpy(path, &tmp_path, sizeof(*path));
    }
    return 0;
  } else {
    *book = tmp_book;
    memcpy(path, &tmp_path, sizeof(*path));
    return 1;
  }
}

void ledger_luaopen_act(struct lua_State *L){
  /* fetch the global `ledger` table */{
    int type = lua_getglobal (L, "ledger");
    if (type != LUA_TTABLE){
      luaL_error(L, "ledger_luaopen_base: bad `ledger` global");
      return;
    }
  }
  ledger_luaopen_actutil(L);
  lua_pop(L,1);
  return;
}

/* END   implementation */
