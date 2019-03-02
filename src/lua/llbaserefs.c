
#include "llbaserefs.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/book.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../base/table.h"
#include <limits.h>
#include <string.h>


static char const* ledger_llbase_account_meta = "ledger.account";
static char const* ledger_llbase_entry_meta = "ledger.entry";
static char const* ledger_llbase_journal_meta = "ledger.journal";
static char const* ledger_llbase_ledger_meta = "ledger.ledger";
static char const* ledger_llbase_book_meta = "ledger.book";

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

/*   BEGIN ledger/base/journal { */

/*
 * `ledger.journal.create()`
 * @return an journal instance
 */
static int ledger_luaL_journal_create(struct lua_State *L);

/*
 * `ledger.journal.__gc(self~ledger.journal)`
 * - self the journal to free
 */
static int ledger_luaL_journal___gc(struct lua_State *L);

/*
 * `ledger.journal.__eq(a~ledger.journal, b~ledger.journal)`
 * - a first journal
 * - b second journal
 * @return true if equal, false otherwise
 */
static int ledger_luaL_journal___eq(struct lua_State *L);

/*
 * `ledger.journal.ptr(self~ledger.journal)`
 * - self the journal to query
 * @return the direct pointer to the journal
 */
static int ledger_luaL_journal_ptr(struct lua_State *L);

/*
 * `ledger.journal.getdescription(self~ledger.journal)`
 * - self the journal to query
 * @return the journal's description
 */
static int ledger_luaL_journal_getdescription(struct lua_State *L);

/*
 * `ledger.journal.setdescription(self~ledger.journal, d~string)`
 * - self the journal to modify
 * - d new description
 * @return a success flag
 */
static int ledger_luaL_journal_setdescription(struct lua_State *L);

/*
 * `ledger.journal.getname(self~ledger.journal)`
 * - self the journal to query
 * @return the journal's name
 */
static int ledger_luaL_journal_getname(struct lua_State *L);

/*
 * `ledger.journal.setname(self~ledger.journal, n~string)`
 * - self the journal to modify
 * - n new name
 * @return a success flag
 */
static int ledger_luaL_journal_setname(struct lua_State *L);

/*
 * `ledger.journal.getid(self~ledger.journal)`
 * - self the journal to query
 * @return the journal's identifier
 */
static int ledger_luaL_journal_getid(struct lua_State *L);

/*
 * `ledger.journal.setid(self~ledger.journal, v~number)`
 * - self the journal to modify
 * - v new identifier
 * @return a success flag
 */
static int ledger_luaL_journal_setid(struct lua_State *L);

/*
 * `ledger.journal.gettable(self~ledger.journal)`
 * - self the journal to query
 * @return the journal's table
 */
static int ledger_luaL_journal_gettable(struct lua_State *L);

/*
 * `ledger.journal.getsequence(self~ledger.journal)`
 * - self the journal to query
 * @return the journal's sequence number
 */
static int ledger_luaL_journal_getsequence(struct lua_State *L);

/*
 * `ledger.journal.setsequence(self~ledger.journal, v~number)`
 * - self the journal to modify
 * - v new sequence number
 * @return a success flag
 */
static int ledger_luaL_journal_setsequence(struct lua_State *L);

/*
 * `ledger.journal.getentry(self~ledger.journal, i~number)`
 * - self the journal to query
 * - i entry array index
 * @return the journal's i-th entry
 */
static int ledger_luaL_journal_getentry(struct lua_State *L);

/*
 * `ledger.journal.getentrycount(self~ledger.journal)`
 * - self the journal to query
 * @return the number of entries in the journal
 */
static int ledger_luaL_journal_getentrycount(struct lua_State *L);

/*
 * `ledger.journal.setentrycount(self~ledger.journal, v~number)`
 * - self the journal to modify
 * - v new entry count
 * @return a success flag
 */
static int ledger_luaL_journal_setentrycount(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_journal_lib[] = {
  {"create", ledger_luaL_journal_create},
  {"__gc", ledger_luaL_journal___gc},
  {"__eq", ledger_luaL_journal___eq},
  {"ptr", ledger_luaL_journal_ptr},
  {"getdescription", ledger_luaL_journal_getdescription},
  {"setdescription", ledger_luaL_journal_setdescription},
  {"getname", ledger_luaL_journal_getname},
  {"setname", ledger_luaL_journal_setname},
  {"getid", ledger_luaL_journal_getid},
  {"setid", ledger_luaL_journal_setid},
  {"gettable", ledger_luaL_journal_gettable},
  {"getsequence", ledger_luaL_journal_getsequence},
  {"setsequence", ledger_luaL_journal_setsequence},
  {"getentrycount", ledger_luaL_journal_getentrycount},
  {"setentrycount", ledger_luaL_journal_setentrycount},
  {"getentry", ledger_luaL_journal_getentry},
  {NULL, NULL}
};

/* } END   ledger/base/journal */

/*   BEGIN ledger/base/ledger { */

/*
 * `ledger.ledger.create()`
 * @return an ledger instance
 */
static int ledger_luaL_ledger_create(struct lua_State *L);

/*
 * `ledger.ledger.__gc(self~ledger.ledger)`
 * - self the ledger to free
 */
static int ledger_luaL_ledger___gc(struct lua_State *L);

/*
 * `ledger.ledger.__eq(a~ledger.ledger, b~ledger.ledger)`
 * - a first ledger
 * - b second ledger
 * @return true if equal, false otherwise
 */
static int ledger_luaL_ledger___eq(struct lua_State *L);

/*
 * `ledger.ledger.ptr(self~ledger.ledger)`
 * - self the ledger to query
 * @return the direct pointer to the ledger
 */
static int ledger_luaL_ledger_ptr(struct lua_State *L);

/*
 * `ledger.ledger.getdescription(self~ledger.ledger)`
 * - self the ledger to query
 * @return the ledger's description
 */
static int ledger_luaL_ledger_getdescription(struct lua_State *L);

/*
 * `ledger.ledger.setdescription(self~ledger.ledger, d~string)`
 * - self the ledger to modify
 * - d new description
 * @return a success flag
 */
static int ledger_luaL_ledger_setdescription(struct lua_State *L);

/*
 * `ledger.ledger.getname(self~ledger.ledger)`
 * - self the ledger to query
 * @return the ledger's name
 */
static int ledger_luaL_ledger_getname(struct lua_State *L);

/*
 * `ledger.ledger.setname(self~ledger.ledger, n~string)`
 * - self the ledger to modify
 * - n new name
 * @return a success flag
 */
static int ledger_luaL_ledger_setname(struct lua_State *L);

/*
 * `ledger.ledger.getid(self~ledger.ledger)`
 * - self the ledger to query
 * @return the ledger's identifier
 */
static int ledger_luaL_ledger_getid(struct lua_State *L);

/*
 * `ledger.ledger.setid(self~ledger.ledger, v~number)`
 * - self the ledger to modify
 * - v new identifier
 * @return a success flag
 */
static int ledger_luaL_ledger_setid(struct lua_State *L);

/*
 * `ledger.ledger.getsequence(self~ledger.ledger)`
 * - self the ledger to query
 * @return the ledger's sequence number
 */
static int ledger_luaL_ledger_getsequence(struct lua_State *L);

/*
 * `ledger.ledger.setsequence(self~ledger.ledger, v~number)`
 * - self the ledger to modify
 * - v new sequence number
 * @return a success flag
 */
static int ledger_luaL_ledger_setsequence(struct lua_State *L);

/*
 * `ledger.ledger.getaccount(self~ledger.ledger, i~number)`
 * - self the ledger to query
 * - i account array index
 * @return the ledger's i-th account
 */
static int ledger_luaL_ledger_getaccount(struct lua_State *L);

/*
 * `ledger.ledger.getaccountcount(self~ledger.ledger)`
 * - self the ledger to query
 * @return the number of entries in the ledger
 */
static int ledger_luaL_ledger_getaccountcount(struct lua_State *L);

/*
 * `ledger.ledger.setaccountcount(self~ledger.ledger, v~number)`
 * - self the ledger to modify
 * - v new account count
 * @return a success flag
 */
static int ledger_luaL_ledger_setaccountcount(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_ledger_lib[] = {
  {"create", ledger_luaL_ledger_create},
  {"__gc", ledger_luaL_ledger___gc},
  {"__eq", ledger_luaL_ledger___eq},
  {"ptr", ledger_luaL_ledger_ptr},
  {"getdescription", ledger_luaL_ledger_getdescription},
  {"setdescription", ledger_luaL_ledger_setdescription},
  {"getname", ledger_luaL_ledger_getname},
  {"setname", ledger_luaL_ledger_setname},
  {"getid", ledger_luaL_ledger_getid},
  {"setid", ledger_luaL_ledger_setid},
  {"getsequence", ledger_luaL_ledger_getsequence},
  {"setsequence", ledger_luaL_ledger_setsequence},
  {"getaccountcount", ledger_luaL_ledger_getaccountcount},
  {"setaccountcount", ledger_luaL_ledger_setaccountcount},
  {"getaccount", ledger_luaL_ledger_getaccount},
  {NULL, NULL}
};

/* } END   ledger/base/ledger */

/*   BEGIN ledger/base/book { */

/*
 * `ledger.book.create()`
 * @return an book instance
 */
static int ledger_luaL_book_create(struct lua_State *L);

/*
 * `ledger.book.__gc(self~ledger.book)`
 * - self the book to free
 */
static int ledger_luaL_book___gc(struct lua_State *L);

/*
 * `ledger.book.__eq(a~ledger.book, b~ledger.book)`
 * - a first book
 * - b second book
 * @return true if equal, false otherwise
 */
static int ledger_luaL_book___eq(struct lua_State *L);

/*
 * `ledger.book.ptr(self~ledger.book)`
 * - self the book to query
 * @return the direct pointer to the book
 */
static int ledger_luaL_book_ptr(struct lua_State *L);

/*
 * `ledger.book.getdescription(self~ledger.book)`
 * - self the book to query
 * @return the book's description
 */
static int ledger_luaL_book_getdescription(struct lua_State *L);

/*
 * `ledger.book.setdescription(self~ledger.book, d~string)`
 * - self the book to modify
 * - d new description
 * @return a success flag
 */
static int ledger_luaL_book_setdescription(struct lua_State *L);

/*
 * `ledger.book.getnotes(self~ledger.book)`
 * - self the book to query
 * @return the book's notes
 */
static int ledger_luaL_book_getnotes(struct lua_State *L);

/*
 * `ledger.book.setnotes(self~ledger.book, n~string)`
 * - self the book to modify
 * - n new notes
 * @return a success flag
 */
static int ledger_luaL_book_setnotes(struct lua_State *L);

/*
 * `ledger.book.getsequence(self~ledger.book)`
 * - self the book to query
 * @return the book's sequence number
 */
static int ledger_luaL_book_getsequence(struct lua_State *L);

/*
 * `ledger.book.setsequence(self~ledger.book, v~number)`
 * - self the book to modify
 * - v new sequence number
 * @return a success flag
 */
static int ledger_luaL_book_setsequence(struct lua_State *L);

/*
 * `ledger.book.getledger(self~ledger.book, i~number)`
 * - self the book to query
 * - i ledger array index
 * @return the book's i-th ledger
 */
static int ledger_luaL_book_getledger(struct lua_State *L);

/*
 * `ledger.book.getledgercount(self~ledger.book)`
 * - self the book to query
 * @return the number of ledgers in the book
 */
static int ledger_luaL_book_getledgercount(struct lua_State *L);

/*
 * `ledger.book.setledgercount(self~ledger.book, v~number)`
 * - self the book to modify
 * - v new ledger count
 * @return a success flag
 */
static int ledger_luaL_book_setledgercount(struct lua_State *L);

/*
 * `ledger.book.getjournal(self~ledger.book, i~number)`
 * - self the book to query
 * - i journal array index
 * @return the book's i-th journal
 */
static int ledger_luaL_book_getjournal(struct lua_State *L);

/*
 * `ledger.book.getjournalcount(self~ledger.book)`
 * - self the book to query
 * @return the number of journals in the book
 */
static int ledger_luaL_book_getjournalcount(struct lua_State *L);

/*
 * `ledger.book.setjournalcount(self~ledger.book, v~number)`
 * - self the book to modify
 * - v new journal count
 * @return a success flag
 */
static int ledger_luaL_book_setjournalcount(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_book_lib[] = {
  {"create", ledger_luaL_book_create},
  {"__gc", ledger_luaL_book___gc},
  {"__eq", ledger_luaL_book___eq},
  {"ptr", ledger_luaL_book_ptr},
  {"getdescription", ledger_luaL_book_getdescription},
  {"setdescription", ledger_luaL_book_setdescription},
  {"getnotes", ledger_luaL_book_getnotes},
  {"setnotes", ledger_luaL_book_setnotes},
  {"getsequence", ledger_luaL_book_getsequence},
  {"setsequence", ledger_luaL_book_setsequence},
  {"getjournalcount", ledger_luaL_book_getjournalcount},
  {"setjournalcount", ledger_luaL_book_setjournalcount},
  {"getjournal", ledger_luaL_book_getjournal},
  {"getledgercount", ledger_luaL_book_getledgercount},
  {"setledgercount", ledger_luaL_book_setledgercount},
  {"getledger", ledger_luaL_book_getledger},
  {NULL, NULL}
};

/* } END   ledger/base/book */

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

/*   BEGIN ledger/base/journal { */

int ledger_luaL_journal_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return~ledger.journal
   * THROW:
   *   X
   */
  struct ledger_journal* next_journal;
  int ok;
  /* execute C API */{
    next_journal = ledger_journal_new();
    if (next_journal != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llbase_postjournal
    (L, next_journal, ok, "ledger.journal.create: generic error");
  return 1;
}

int ledger_luaL_journal___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   X
   */
  struct ledger_journal** a =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  ledger_journal_free(*a);
  *a = NULL;
  return 0;
}

int ledger_luaL_journal_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.journal
   *   2  right~ledger.journal
   * ...:
   */
  struct ledger_journal** left =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  struct ledger_journal** right =
    (struct ledger_journal**)luaL_checkudata
        (L, 2, ledger_llbase_journal_meta);
  if (*left != NULL && *right != NULL){
    return ledger_journal_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_journal_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_journal___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.journal
   *   2  right~ledger.journal
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_journal_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_journal_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  lua_pushlightuserdata(L, (void*)*j);
  return 1;
}

int ledger_luaL_journal_getdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~string
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  unsigned char const* d = ledger_journal_get_description(*j);
  lua_pushstring(L, (char const*)d);
  return 1;
}

int ledger_luaL_journal_setdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   *   2  d~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  unsigned char const* d = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_journal_set_description(*j, d);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_journal_getname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~string
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  unsigned char const* n = ledger_journal_get_name(*j);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_journal_setname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_journal_set_name(*j, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_journal_getid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~number
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int v = ledger_journal_get_id(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_journal_setid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int const v = (int)lua_tointeger(L, 2);
  ledger_journal_set_id(*j, v);
  lua_pushboolean(L, 1);
  return 1;
}

int ledger_luaL_journal_gettable(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~ledger.table
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  struct ledger_table* t = ledger_journal_get_table(*j);
  if (ledger_table_acquire(t) != t){
    luaL_error(L, "ledger.journal.gettable: Table unavailable");
  } else {
    ledger_llbase_posttable
      (L, t, 1, "ledger.journal.gettable: Table available");
  }
  return 1;
}

int ledger_luaL_journal_getsequence(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~number
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int v = ledger_journal_get_sequence(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_journal_setsequence(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_journal_set_sequence(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_journal_getentry(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   *   2  v~number
   * RET:
   *   3  @return~ledger.entry
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int const v = ((int)lua_tointeger(L, 2))-1;
  struct ledger_entry* e = ledger_journal_get_entry(*j, v);
  if (e == NULL){
    lua_pushnil(L);
  } else if (ledger_entry_acquire(e) != e){
    luaL_error(L, "ledger.journal.getentry: Journal entry unavailable");
  } else {
    ledger_llbase_postentry
      (L, e, 1, "ledger.journal.getentry: Journal entry available");
  }
  return 1;
}

int ledger_luaL_journal_getentrycount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   * RET:
   *   2  @return~number
   */
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int v = ledger_journal_get_entry_count(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_journal_setentrycount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.journal
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_journal** j =
    (struct ledger_journal**)luaL_checkudata
        (L, 1, ledger_llbase_journal_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_journal_set_entry_count(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

/* } END   ledger/base/journal */

/*   BEGIN ledger/base/ledger { */

int ledger_luaL_ledger_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return~ledger.ledger
   * THROW:
   *   X
   */
  struct ledger_ledger* next_ledger;
  int ok;
  /* execute C API */{
    next_ledger = ledger_ledger_new();
    if (next_ledger != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llbase_postledger
    (L, next_ledger, ok, "ledger.ledger.create: generic error");
  return 1;
}

int ledger_luaL_ledger___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   X
   */
  struct ledger_ledger** a =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  ledger_ledger_free(*a);
  *a = NULL;
  return 0;
}

int ledger_luaL_ledger_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.ledger
   *   2  right~ledger.ledger
   * ...:
   */
  struct ledger_ledger** left =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  struct ledger_ledger** right =
    (struct ledger_ledger**)luaL_checkudata
        (L, 2, ledger_llbase_ledger_meta);
  if (*left != NULL && *right != NULL){
    return ledger_ledger_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_ledger_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_ledger___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.ledger
   *   2  right~ledger.ledger
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_ledger_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_ledger_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  lua_pushlightuserdata(L, (void*)*j);
  return 1;
}

int ledger_luaL_ledger_getdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   2  @return~string
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  unsigned char const* d = ledger_ledger_get_description(*j);
  lua_pushstring(L, (char const*)d);
  return 1;
}

int ledger_luaL_ledger_setdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   *   2  d~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  unsigned char const* d = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_ledger_set_description(*j, d);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_ledger_getname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   2  @return~string
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  unsigned char const* n = ledger_ledger_get_name(*j);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_ledger_setname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_ledger_set_name(*j, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_ledger_getid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   2  @return~number
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int v = ledger_ledger_get_id(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_ledger_setid(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int const v = (int)lua_tointeger(L, 2);
  ledger_ledger_set_id(*j, v);
  lua_pushboolean(L, 1);
  return 1;
}

int ledger_luaL_ledger_getsequence(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   2  @return~number
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int v = ledger_ledger_get_sequence(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_ledger_setsequence(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_ledger_set_sequence(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_ledger_getaccount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   *   2  v~number
   * RET:
   *   3  @return~ledger.account
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int const v = ((int)lua_tointeger(L, 2))-1;
  struct ledger_account* e = ledger_ledger_get_account(*j, v);
  if (e == NULL){
    lua_pushnil(L);
  } else if (ledger_account_acquire(e) != e){
    luaL_error(L, "ledger.ledger.getaccount: Account unavailable");
  } else {
    ledger_llbase_postaccount
      (L, e, 1, "ledger.ledger.getaccount: Account available");
  }
  return 1;
}

int ledger_luaL_ledger_getaccountcount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   * RET:
   *   2  @return~number
   */
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int v = ledger_ledger_get_account_count(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_ledger_setaccountcount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.ledger
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_ledger** j =
    (struct ledger_ledger**)luaL_checkudata
        (L, 1, ledger_llbase_ledger_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_ledger_set_account_count(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

/* } END   ledger/base/ledger */

/*   BEGIN ledger/base/book { */

int ledger_luaL_book_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return~ledger.book
   * THROW:
   *   X
   */
  struct ledger_book* next_book;
  int ok;
  /* execute C API */{
    next_book = ledger_book_new();
    if (next_book != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llbase_postbook
    (L, next_book, ok, "ledger.book.create: generic error");
  return 1;
}

int ledger_luaL_book___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   X
   */
  struct ledger_book** a =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  ledger_book_free(*a);
  *a = NULL;
  return 0;
}

int ledger_luaL_book_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.book
   *   2  right~ledger.book
   * ...:
   */
  struct ledger_book** left =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  struct ledger_book** right =
    (struct ledger_book**)luaL_checkudata
        (L, 2, ledger_llbase_book_meta);
  if (*left != NULL && *right != NULL){
    return ledger_book_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_book_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_book___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.book
   *   2  right~ledger.book
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_book_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_book_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  lua_pushlightuserdata(L, (void*)*j);
  return 1;
}

int ledger_luaL_book_getdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2  @return~string
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  unsigned char const* d = ledger_book_get_description(*j);
  lua_pushstring(L, (char const*)d);
  return 1;
}

int ledger_luaL_book_setdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  d~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  unsigned char const* d = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_book_set_description(*j, d);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_book_getnotes(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2  @return~string
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  unsigned char const* n = ledger_book_get_notes(*j);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_book_setnotes(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_book_set_notes(*j, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_book_getsequence(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2  @return~number
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int v = ledger_book_get_sequence(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_book_setsequence(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_book_set_sequence(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_book_getledger(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  v~number
   * RET:
   *   3  @return~ledger.ledger
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int const v = ((int)lua_tointeger(L, 2))-1;
  struct ledger_ledger* e = ledger_book_get_ledger(*j, v);
  if (e == NULL){
    lua_pushnil(L);
  } else if (ledger_ledger_acquire(e) != e){
    luaL_error(L, "ledger.book.getledger: Journal ledger unavailable");
  } else {
    ledger_llbase_postledger
      (L, e, 1, "ledger.book.getledger: Journal ledger available");
  }
  return 1;
}

int ledger_luaL_book_getledgercount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2  @return~number
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int v = ledger_book_get_ledger_count(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_book_setledgercount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_book_set_ledger_count(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_book_getjournal(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  v~number
   * RET:
   *   3  @return~ledger.journal
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int const v = ((int)lua_tointeger(L, 2))-1;
  struct ledger_journal* e = ledger_book_get_journal(*j, v);
  if (e == NULL){
    lua_pushnil(L);
  } else if (ledger_journal_acquire(e) != e){
    luaL_error(L, "ledger.book.getjournal: Journal journal unavailable");
  } else {
    ledger_llbase_postjournal
      (L, e, 1, "ledger.book.getjournal: Journal journal available");
  }
  return 1;
}

int ledger_luaL_book_getjournalcount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   * RET:
   *   2  @return~number
   */
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int v = ledger_book_get_journal_count(*j);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_book_setjournalcount(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.book
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_book** j =
    (struct ledger_book**)luaL_checkudata
        (L, 1, ledger_llbase_book_meta);
  int const v = (int)lua_tointeger(L, 2);
  result = ledger_book_set_journal_count(*j, v);
  lua_pushboolean(L, result);
  return 1;
}

/* } END   ledger/base/book */

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
  /* add journal lib */{
    luaL_newlib(L, ledger_luaL_journal_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_journal_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_journal_meta);
    }
    lua_setfield(L, -2, "journal");
  }
  /* add ledger lib */{
    luaL_newlib(L, ledger_luaL_ledger_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_ledger_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_ledger_meta);
    }
    lua_setfield(L, -2, "ledger");
  }
  /* add book lib */{
    luaL_newlib(L, ledger_luaL_book_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_book_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_book_meta);
    }
    lua_setfield(L, -2, "book");
  }
  return;
}

/* END   implementation */
