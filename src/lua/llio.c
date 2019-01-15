
#include "llio.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../io/book.h"
#include "../base/book.h"

static char const* ledger_llbase_book_meta = "ledger.book";

/*   BEGIN ledger/io/book { */

/*
 * `ledger.io.readbook(fn)`
 * - fn name of book file to read
 * @return the book on success, nil otherwise
 */
static int ledger_luaL_io_readbook(struct lua_State *L);

/*
 * `ledger.io.writebook(fn, b~ledger.book)`
 * - fn name of book file to write
 * - b book to write to file
 * @return a success flag
 */
static int ledger_luaL_io_writebook(struct lua_State *L);

/* } END   ledger/io/book */

static const struct luaL_Reg ledger_luaL_io_lib[] = {
  {"readbook", ledger_luaL_io_readbook},
  {"writebook", ledger_luaL_io_writebook},
  {NULL,NULL}
};

/* BEGIN static implementation */

int ledger_luaL_io_readbook(struct lua_State *L){
  /* ARG:
   *   1  fn~string
   * RET:
   *   2 @return~ledger.book
   * THROW:
   *   X
   */
  int ok;
  struct ledger_book* next_book;
  char const* fn = lua_tostring(L, 1);
  /* execute C API */{
    next_book = ledger_book_new();
    if (next_book != NULL){
      ok = ledger_io_book_read(fn, next_book);
    } else ok = 0;
  }
  if (ok){
    ledger_llbase_postbook
        (L, next_book, 1, "ledger.io.readbook: success");
  } else {
    ledger_book_free(next_book);
    lua_pushnil(L);
  }
  return 1;
}

int ledger_luaL_io_writebook(struct lua_State *L){
  /* ARG:
   *   1  fn~string
   *   2  b~ledger.book
   * RET:
   *   3 @return~boolean
   * THROW:
   *   X
   */
  int ok;
  char const* fn = lua_tostring(L, 1);
  struct ledger_book** b =
    (struct ledger_book**)luaL_checkudata
        (L, 2, ledger_llbase_book_meta);
  /* execute C API */{
    ok = ledger_io_book_write(fn, *b);
  }
  lua_pushboolean(L, ok);
  return 1;
}

/* END   static implementation */

/* BEGIN implementation */

void ledger_luaopen_io(struct lua_State *L){
  /* fetch the global `ledger` table */{
    int type = lua_getglobal (L, "ledger");
    if (type != LUA_TTABLE){
      luaL_error(L, "ledger_luaopen_io: bad `ledger` global");
      return;
    }
  }
  /* add io lib */{
    luaL_newlib(L, ledger_luaL_io_lib);
    lua_setfield(L, -2, "io");
  }
  lua_pop(L,1);
  return;
}

/* END   implementation */
