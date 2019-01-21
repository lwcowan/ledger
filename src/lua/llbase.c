
#include "llbaseutil.h"
#include "llbasetable.h"
#include "llbaserefs.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../base/table.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/book.h"
#include <limits.h>


/* [INTERNAL]
 * Post the string on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_poststringP1(struct lua_State *L);

/* [INTERNAL]
 * Post the big number on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_postbignumP1(struct lua_State *L);

/* [INTERNAL]
 * Post the table on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_posttableP1(struct lua_State *L);

/* [INTERNAL]
 * Post the table mark on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_posttablemarkP1(struct lua_State *L);

/* [INTERNAL]
 * Post the account on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_postaccountP1(struct lua_State *L);

/* [INTERNAL]
 * Post the entry on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_postentryP1(struct lua_State *L);

/* [INTERNAL]
 * Post the journal on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_postjournalP1(struct lua_State *L);

/* [INTERNAL]
 * Post the ledger on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_postledgerP1(struct lua_State *L);

/* [INTERNAL]
 * Post the book on the top of the stack.
 * - L lua state to modify
 * @return one
 */
static int ledger_llbase_postbookP1(struct lua_State *L);

static char const* ledger_llbase_bignum_meta = "ledger.bignum";
static char const* ledger_llbase_table_meta = "ledger.table";
static char const* ledger_llbase_tablemark_meta = "ledger.table.mark";
static char const* ledger_llbase_account_meta = "ledger.account";
static char const* ledger_llbase_entry_meta = "ledger.entry";
static char const* ledger_llbase_journal_meta = "ledger.journal";
static char const* ledger_llbase_ledger_meta = "ledger.ledger";
static char const* ledger_llbase_book_meta = "ledger.book";

/* BEGIN static implementation */

int ledger_llbase_poststringP1(struct lua_State *L){
  /* ARG:
   *   1 *str
   * RET:
   *   2 @return
   */
  unsigned char const* str = (unsigned char const*)lua_touserdata(L, 1);
  if (str != NULL)
    lua_pushstring(L, (char const*)str);
  else
    lua_pushnil(L);
  return 1;
}

int ledger_llbase_postbignumP1(struct lua_State *L){
  /* ARG:
   *   1 *bignum
   * RET:
   *   2 @return
   */
  struct ledger_bignum* num = (struct ledger_bignum*)lua_touserdata(L, 1);
  /* get a full user data for the big number pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.bignum`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = num;
  /* apply the userdata registry item `ledger.bignum` to this userdata */
  luaL_setmetatable(L, ledger_llbase_bignum_meta);
  return 1;
}

int ledger_llbase_posttableP1(struct lua_State *L){
  /* ARG:
   *   1 *table
   * RET:
   *   2 @return
   */
  struct ledger_table* t = (struct ledger_table*)lua_touserdata(L, 1);
  /* get a full user data for the table pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.table`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = t;
  /* apply the userdata registry item `ledger.table` to this userdata */
  luaL_setmetatable(L, ledger_llbase_table_meta);
  return 1;
}

int ledger_llbase_posttablemarkP1(struct lua_State *L){
  /* ARG:
   *   1 *tablemark
   * RET:
   *   2 @return
   */
  struct ledger_table_mark* num =
    (struct ledger_table_mark*)lua_touserdata(L, 1);
  /* get a full user data for the table mark pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.table.mark`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = num;
  /* apply the userdata registry item `ledger.table.mark` to this userdata */
  luaL_setmetatable(L, ledger_llbase_tablemark_meta);
  return 1;
}

int ledger_llbase_postaccountP1(struct lua_State *L){
  /* ARG:
   *   1 *account
   * RET:
   *   2 @return
   */
  struct ledger_account* a = (struct ledger_account*)lua_touserdata(L, 1);
  /* get a full user data for the account pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.account`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = a;
  /* apply the userdata registry item `ledger.account` to this userdata */
  luaL_setmetatable(L, ledger_llbase_account_meta);
  return 1;
}

int ledger_llbase_postentryP1(struct lua_State *L){
  /* ARG:
   *   1 *entry
   * RET:
   *   2 @return
   */
  struct ledger_entry* e = (struct ledger_entry*)lua_touserdata(L, 1);
  /* get e full user data for the entry pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.entry`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = e;
  /* apply the userdata registry item `ledger.entry` to this userdata */
  luaL_setmetatable(L, ledger_llbase_entry_meta);
  return 1;
}

int ledger_llbase_postjournalP1(struct lua_State *L){
  /* ARG:
   *   1 *journal
   * RET:
   *   2 @return
   */
  struct ledger_journal* a = (struct ledger_journal*)lua_touserdata(L, 1);
  /* get a full user data for the journal pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.journal`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = a;
  /* apply the userdata registry item `ledger.journal` to this userdata */
  luaL_setmetatable(L, ledger_llbase_journal_meta);
  return 1;
}

int ledger_llbase_postledgerP1(struct lua_State *L){
  /* ARG:
   *   1 *ledger
   * RET:
   *   2 @return
   */
  struct ledger_ledger* a = (struct ledger_ledger*)lua_touserdata(L, 1);
  /* get a full user data for the ledger pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.ledger`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = a;
  /* apply the userdata registry item `ledger.ledger` to this userdata */
  luaL_setmetatable(L, ledger_llbase_ledger_meta);
  return 1;
}

int ledger_llbase_postbookP1(struct lua_State *L){
  /* ARG:
   *   1 *book
   * RET:
   *   2 @return
   */
  struct ledger_book* a = (struct ledger_book*)lua_touserdata(L, 1);
  /* get a full user data for the book pointer */
  void ** ptr = (void**)lua_newuserdata(L, sizeof(void*));
  if (ptr == NULL){
    luaL_error(L, "unable to allocate space for `ledger.book`"
      " indirect pointer");
    return 0;
  }
  (*ptr) = a;
  /* apply the userdata registry item `ledger.book` to this userdata */
  luaL_setmetatable(L, ledger_llbase_book_meta);
  return 1;
}

/* END   static implementation */

/* BEGIN implementation */

int ledger_llbase_poststring
  (struct lua_State *L, unsigned char* str, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_poststringP1);
    lua_pushlightuserdata(L, str);
    success_line = lua_pcall(L, 1, 1, 0);
    ledger_util_free(str);
    if (success_line != LUA_OK){
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_util_free(str);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_postbignum
  (struct lua_State *L, struct ledger_bignum* bn, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_postbignumP1);
    lua_pushlightuserdata(L, bn);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_bignum_free(bn);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_bignum_free(bn);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_posttable
  (struct lua_State *L, struct ledger_table* t, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_posttableP1);
    lua_pushlightuserdata(L, t);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_table_free(t);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_table_free(t);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_posttablemark
  (struct lua_State *L, struct ledger_table_mark* t, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_posttablemarkP1);
    lua_pushlightuserdata(L, t);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_table_mark_free(t);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_table_mark_free(t);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_postaccount
  (struct lua_State *L, struct ledger_account* a, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_postaccountP1);
    lua_pushlightuserdata(L, a);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_account_free(a);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_account_free(a);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_postentry
  (struct lua_State *L, struct ledger_entry* e, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_postentryP1);
    lua_pushlightuserdata(L, e);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_entry_free(e);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_entry_free(e);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_postjournal
  (struct lua_State *L, struct ledger_journal* a, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_postjournalP1);
    lua_pushlightuserdata(L, a);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_journal_free(a);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_journal_free(a);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_postledger
  (struct lua_State *L, struct ledger_ledger* a, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_postledgerP1);
    lua_pushlightuserdata(L, a);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_ledger_free(a);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_ledger_free(a);
    luaL_error(L, err);
  }
  return 1;
}

int ledger_llbase_postbook
  (struct lua_State *L, struct ledger_book* a, int ok, char const* err)
{
  /* enter protection */if (ok){
    int success_line;
    lua_pushcfunction(L, ledger_llbase_postbookP1);
    lua_pushlightuserdata(L, a);
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      ledger_book_free(a);
      /* throw; */
      lua_error(L);
    } else /* leave return at top of stack */;
  } else {
    ledger_book_free(a);
    luaL_error(L, err);
  }
  return 1;
}

void ledger_luaopen_base(struct lua_State *L){
  ledger_luaopen_baseutil(L);
  ledger_luaopen_basetable(L);
  ledger_luaopen_baserefs(L);
  return;
}

/* END   implementation */
