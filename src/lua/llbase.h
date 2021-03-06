/*
 * file: act/llbase.h
 * brief: Lua extension for base library API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_lua_LLBase_H__
#define __Ledger_lua_LLBase_H__


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_bignum;
struct ledger_table;
struct ledger_table_mark;
struct ledger_account;
struct ledger_entry;
struct ledger_journal;
struct ledger_ledger;
struct ledger_book;

/*
 * Lua state
 */
struct lua_State;


/*
 * Post the string on the top of the stack.
 * - L lua state to modify
 * - str string, freed by this function if `ok`
 * - ok if nonzero, the string is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_poststring
  (struct lua_State *L, unsigned char* str, int ok, char const* err);

/*
 * Post the given big number to the top of the stack.
 * - L lua state to modify
 * - bn number to post, freed by this function if `ok`
 * - ok if nonzero, the number is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_postbignum
  (struct lua_State *L, struct ledger_bignum* bn, int ok, char const* err);

/*
 * Post the given table to the top of the stack.
 * - L lua state to modify
 * - t table to post
 * - ok if nonzero, the string is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_posttable
  (struct lua_State *L, struct ledger_table* t, int ok, char const* err);

/*
 * Post the given table mark to the top of the stack.
 * - L lua state to modify
 * - tm table mark to post
 * - ok if nonzero, the string is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_posttablemark
  ( struct lua_State *L, struct ledger_table_mark* tm,
    int ok, char const* err);

/*
 * Post the given account to the top of the stack.
 * - L lua state to modify
 * - a account to post
 * - ok if nonzero, the account is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_postaccount
  (struct lua_State *L, struct ledger_account* a, int ok, char const* err);

/*
 * Post the given journal entry to the top of the stack.
 * - L lua state to modify
 * - e journal entry to post
 * - ok if nonzero, the entry is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_postentry
  (struct lua_State *L, struct ledger_entry* e, int ok, char const* err);

/*
 * Post the given journal to the top of the stack.
 * - L lua state to modify
 * - j journal to post
 * - ok if nonzero, the journal is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_postjournal
  (struct lua_State *L, struct ledger_journal* j, int ok, char const* err);

/*
 * Post the given ledger to the top of the stack.
 * - L lua state to modify
 * - j ledger to post
 * - ok if nonzero, the ledger is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_postledger
  (struct lua_State *L, struct ledger_ledger* j, int ok, char const* err);

/*
 * Post the given book to the top of the stack.
 * - L lua state to modify
 * - b book to post
 * - ok if nonzero, the book is posted, else the error is raised
 * - err error message if not `ok`
 * @return one on success, zero otherwise
 */
int ledger_llbase_postbook
  (struct lua_State *L, struct ledger_book* b, int ok, char const* err);

/*
 * Open the base library.
 * @param l native lua state
 */
void ledger_luaopen_base(struct lua_State *l);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_lua_LLBase_H__*/
