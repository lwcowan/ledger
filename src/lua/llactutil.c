
#include "llactutil.h"
#include "llact.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../act/path.h"
#include "../act/select.h"
#include "../act/transact.h"
#include "../base/book.h"
#include "../base/util.h"
#include "../base/table.h"
#include <limits.h>
#include <string.h>
#include <ctype.h>


static char const* ledger_llact_path_meta = "ledger.path";
static char const* ledger_llbase_book_meta = "ledger.book";
static char const* ledger_llbase_table_meta = "ledger.table";
static char const* ledger_llact_transaction_meta = "ledger.transaction";

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

/*
 * `ledger.select.bycond(t~ledger.table, cb~function(~ledger.table.mark),
 *     cond~table{~table}, dir~number)`
 * - t table in which to search
 * - cb coroutine to which to call back on each matching row
 * - cond array of condition structures
 * - dir search direction (negative -> from end; positive -> from start)
 * @return negative one on error, or the first nonzero value from the
 *   callback, zero otherwise
 * @throw carried through from the callback
 */
static int ledger_luaL_select_bycond(struct lua_State *L);

/* [INTERNAL]
 * Fill a condition array from a table of Lua tables.
 * - L Lua state to configure
 */
static int ledger_luaL_select_bycond_fill(struct lua_State *L);

/* [INTERNAL]
 * Convert a comparison string to a comparator code.
 * - str the string to convert
 * @return a corresponding code, or -1 if the string is invalid
 */
static int ledger_luaL_select_cond_atoi(char const* str);

/* [INTERNAL]
 * Marshal the Lua callback.
 * - arg pointer to carry structure
 * - m next mark
 * @return zero to continue, nonzero when done
 */
static int ledger_luaL_select_cb
  (void* arg, struct ledger_table_mark const* m);

static const struct luaL_Reg ledger_luaL_select_lib[] = {
  {"cond", ledger_luaL_select_cond},
  {"bycond", ledger_luaL_select_bycond},
  {NULL,NULL}
};

/* callback argument for ledger_select_by_cond */
struct ledger_luaL_select_bycond_carry {
  /* Lua state */
  struct lua_State *L;
  /* callback stack index */
  int cb_index;
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
  { LEDGER_SELECT_STRING, "string" },
  { LEDGER_SELECT_STRING, "ustr" } /* to match ledger.table API */
};

/* } END   ledger/act/select */

/*   BEGIN ledger/act/transaction { */

/*
 * `ledger.transaction.create()`
 * @return the transaction object
 */
int ledger_luaL_transaction_create(struct lua_State* L);

/*
 * `ledger.transaction.__gc(t~ledger.transaction)`
 * - t transaction to collect
 */
int ledger_luaL_transaction___gc(struct lua_State* L);

/*
 * `ledger.transaction.__eq(a~ledger.transaction, b~ledger.transaction)`
 * - a first transaction
 * - b second transaction
 * @return whether the transactions are equivalent
 */
int ledger_luaL_transaction___eq(struct lua_State* L);

/*
 * `ledger.transaction.ptr(t~ledger.transaction)`
 * - t transaction to process
 * @return the C pointer for the transaction
 */
int ledger_luaL_transaction_ptr(struct lua_State* L);

/*
 * `ledger.transaction.getdescription(t~ledger.transaction)`
 * - t transaction to query
 * @return its description
 */
int ledger_luaL_transaction_getdescription(struct lua_State* L);

/*
 * `ledger.transaction.setdescription(t~ledger.transaction, s~string)`
 * - t transaction to configure
 * - s new description
 * @return one on success, zero otherwise
 */
int ledger_luaL_transaction_setdescription(struct lua_State* L);

/*
 * `ledger.transaction.getname(t~ledger.transaction)`
 * - t transaction to query
 * @return the name for the resulting entry
 */
int ledger_luaL_transaction_getname(struct lua_State* L);

/*
 * `ledger.transaction.setname(t~ledger.transaction, s~string)`
 * - t transaction to configure
 * - s new name for the resulting entry
 * @return one on success, zero otherwise
 */
int ledger_luaL_transaction_setname(struct lua_State* L);

/*
 * `ledger.transaction.getdate(t~ledger.transaction)`
 * - t the transaction to query
 * @return the date currently set on the transaction
 */
int ledger_luaL_transaction_getdate(struct lua_State* L);

/*
 * `ledger.transaction.setdate(t~ledger.transaction, d~string)`
 * - t transaction to configure
 * - d date to set on the new entry
 * @return one on success, zero otherwise
 */
int ledger_luaL_transaction_setdate(struct lua_State* L);

/*
 * `ledger.transaction.getjournal(t~ledger.transaction)`
 * - t transaction to query
 * @return the array index of the journal into which to write
 */
int ledger_luaL_transaction_getjournal(struct lua_State* L);

/*
 * `ledger.transaction.setjournal(t~ledger.transaction, j~number)`
 * - t transaction to configure
 * - j array index of journal into which to write
 * @return one on success, zero otherwise
 */
int ledger_luaL_transaction_setjournal(struct lua_State* L);

/*
 * `ledger.transaction.gettable(t~ledger.transaction)`
 * - t transaction to configure
 * @return a reference to the transaction line table
 */
int ledger_luaL_transaction_gettable(struct lua_State* L);

static const struct luaL_Reg ledger_luaL_transaction_lib[] = {
  {"create", ledger_luaL_transaction_create},
  {"__gc", ledger_luaL_transaction___gc},
  {"__eq", ledger_luaL_transaction___eq},
  {"ptr", ledger_luaL_transaction_ptr},
  {"getdescription", ledger_luaL_transaction_getdescription},
  {"setdescription", ledger_luaL_transaction_setdescription},
  {"getname", ledger_luaL_transaction_getname},
  {"setname", ledger_luaL_transaction_setname},
  {"getdate", ledger_luaL_transaction_getdate},
  {"setdate", ledger_luaL_transaction_setdate},
  {"getjournal", ledger_luaL_transaction_getjournal},
  {"setjournal", ledger_luaL_transaction_setjournal},
  {"gettable", ledger_luaL_transaction_gettable},
  {NULL,NULL}
};

/* } END   ledger/act/transaction */

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

int ledger_luaL_select_cb
  (void* arg, struct ledger_table_mark const* m)
{
  int zero_check = 0;
  struct ledger_luaL_select_bycond_carry const* carry =
    (struct ledger_luaL_select_bycond_carry const*)arg;
  struct lua_State *const L = carry->L;
  lua_pushvalue(L, carry->cb_index);
  ledger_table_mark_acquire((struct ledger_table_mark*)m);
  /* marshal the mark */{
    if (!ledger_llbase_posttablemark
      ( L, (struct ledger_table_mark*)m,
        1, "ledger.select.bycond: success"))
    {
      return 1;
    }
  }
  /* protectedly call the function */{
    int success_line;
    success_line = lua_pcall(L, 1, 1, 0);
    if (success_line != LUA_OK){
      /* leave the error on top of the Lua stack */
      return 2;
    } else /* inspect the return value */{
      switch (lua_type(L, -1)){
      case LUA_TNIL:
      case LUA_TNONE:
        zero_check = 1;
        break;
      case LUA_TBOOLEAN:
        if (lua_toboolean(L, -1) == 0)
          zero_check = 1;
        break;
      case LUA_TNUMBER:
        if (lua_tonumber(L, -1) == 0.0)
          zero_check = 1;
        break;
      }
      /* move the new value into place */
      lua_rotate(L, lua_absindex(L, -2), -1);
      lua_pop(L, 1);
    }
  }
  /* return a value for the ledger API */
  if (zero_check)
    return 0;
  else
    return 3;
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
        p = q-1;
      } else break;
    }
  }
  if (*p == 0)
    return code;
  else
    return -1;
}

int ledger_luaL_select_bycond_fill(struct lua_State *L){
  /* ARG:
   *   1  *ledger_select_cond[len]
   *   2  len
   *   3  ~table
   * RET:
   *   X
   * THROW:
   *   X
   */
  int cond_len = lua_tointeger(L, 2);
  struct ledger_select_cond *const cond_array =
      (struct ledger_select_cond *)lua_touserdata(L, 1);
  int i;
  for (i = 0; i < cond_len; ++i){
    lua_geti(L, 3, i+1);
    if (!lua_isnil(L, -1)){
      char const* value_text;
      unsigned char* renew_text;
      /* fill the condition structure */
      struct ledger_select_cond *const next_cond = cond_array+i;
      lua_getfield(L, -1, "cmp");
      next_cond->cmp = (int)lua_tointeger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "column");
      next_cond->column = (int)(lua_tointeger(L, -1)-1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "value");
      value_text = lua_tostring(L, -1);
      if (value_text == NULL){
        luaL_error(L, "ledger.select.bycond: missing value for comparison");
      }
      renew_text =
        ledger_util_ustrdup((unsigned char const*)value_text, NULL);
      if (renew_text == NULL){
        luaL_error(L, "ledger.select.bycond: low memory encountered");
      }
      next_cond->value = renew_text;
      lua_pop(L, 1);
    }
    lua_pop(L, 1);
  }
  return 0;
}

int ledger_luaL_select_bycond(struct lua_State *L){
  /* ARG:
   *   1  t~ledger.table
   *   2  cb~function(~ledger.table.mark)
   *   3  cond~table{~table}
   *   4  dir~number
   * RET:
   *   5 @return~number
   * THROW:
   *   6 @throw from cb
   */
  struct ledger_luaL_select_bycond_carry carry;
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  lua_Integer dir = lua_tointeger(L, 4);
  struct ledger_select_cond *cond_array;
  int cond_len;
  int execution_result;
  /* compose the condition array */{
    lua_Integer intended_cond_len;
    lua_len(L, 3);
    intended_cond_len = lua_tointeger(L, -1);
    if (intended_cond_len < 0) cond_len = 0;
    else if (intended_cond_len >= INT_MAX/sizeof(*cond_array)){
      luaL_error(L, "ledger.select.bycond: condition array to large");
    } else cond_len = intended_cond_len;
    cond_array = ledger_util_malloc(cond_len *sizeof(*cond_array));
    if (cond_array == NULL){
      luaL_error(L,
        "ledger.select.bycond: condition array to large to allocate");
    }
    /* pre-fill the strings of the condition array */{
      int i;
      for (i = 0; i < cond_len; ++i){
        cond_array[i].value = NULL;
      }
    }
    /* fill the condition array */{
      int success_line;
      lua_pushcfunction(L, ledger_luaL_select_bycond_fill);
      lua_pushlightuserdata(L, cond_array);
      lua_pushinteger(L, cond_len);
      lua_pushvalue(L, 3);
      success_line = lua_pcall(L, 3, 0, 0);
      if (success_line != LUA_OK){
        int i;
        for (i = 0; i < cond_len; ++i){
          ledger_util_free(cond_array[i].value);
        }
        ledger_util_free(cond_array);
        lua_error(L);
      }
    }
  }
  /* compose the callback structure */{
    carry.L = L;
    carry.cb_index = 2;
  }
  /* prepare return value space */
  lua_pushinteger(L, 0);
  /* execute C API */{
    execution_result =
      ledger_select_by_cond(*t, &carry, ledger_luaL_select_cb,
          cond_len, cond_array, dir<0?-1:(dir>0?+1:0));
  }
  /* release the condition array */{
    int i;
    for (i = 0; i < cond_len; ++i){
      ledger_util_free(cond_array[i].value);
    }
    ledger_util_free(cond_array);
    cond_array = NULL;
  }
  if (execution_result < 0){
    /* error in ledger API */
    luaL_error(L,
        "ledger.select.bycond: execution error");
  } else if (execution_result == 1){
    /* error in leger-lua callback */
    luaL_error(L,
        "ledger.select.bycond: transfer error");
  } else if (execution_result == 2){
    /* Lua's error */
    lua_error(L);
  } else {
    /* done */
  }
  return 1;
}

/* } END   ledger/act/select */


/*   BEGIN ledger/act/transaction { */

int ledger_luaL_transaction_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return~ledger.transaction
   * THROW:
   *   X
   */
  struct ledger_transaction* next_transaction;
  int ok;
  /* execute C API */{
    next_transaction = ledger_transaction_new();
    if (next_transaction != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llact_posttransaction
    (L, next_transaction, ok, "ledger.transaction.create: generic error");
  return 1;
}

int ledger_luaL_transaction___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   X
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  ledger_transaction_free(*a);
  *a = NULL;
  return 0;
}

int ledger_luaL_transaction_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.transaction
   *   2  right~ledger.transaction
   * ...:
   */
  struct ledger_transaction** left =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  struct ledger_transaction** right =
    (struct ledger_transaction**)luaL_checkudata
        (L, 2, ledger_llact_transaction_meta);
  if (*left != NULL && *right != NULL){
    return ledger_transaction_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L,
      "ledger_luaL_transaction_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_transaction___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.transaction
   *   2  right~ledger.transaction
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_transaction_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_transaction_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  lua_pushlightuserdata(L, (void*)*a);
  return 1;
}

int ledger_luaL_transaction_getdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   2  @return~string
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  unsigned char const* d = ledger_transaction_get_description(*a);
  lua_pushstring(L, (char const*)d);
  return 1;
}

int ledger_luaL_transaction_setdescription(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   *   2  d~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  unsigned char const* d = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_transaction_set_description(*a, d);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_transaction_getname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   2  @return~string
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  unsigned char const* n = ledger_transaction_get_name(*a);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_transaction_setname(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_transaction_set_name(*a, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_transaction_getdate(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   2  @return~string
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  unsigned char const* n = ledger_transaction_get_date(*a);
  lua_pushstring(L, (char const*)n);
  return 1;
}

int ledger_luaL_transaction_setdate(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   *   2  n~string
   * RET:
   *   3  @return~boolean
   */
  int result;
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  unsigned char const* n = (unsigned char const*)lua_tostring(L, 2);
  result = ledger_transaction_set_date(*a, n);
  lua_pushboolean(L, result);
  return 1;
}

int ledger_luaL_transaction_getjournal(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   2  @return~number
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  int v = ledger_transaction_get_journal(*a);
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

int ledger_luaL_transaction_setjournal(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   *   2  v~number
   * RET:
   *   3  @return~boolean
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  int const v = (int)lua_tointeger(L, 2);
  ledger_transaction_set_journal(*a, v);
  lua_pushboolean(L, 1);
  return 1;
}

int ledger_luaL_transaction_gettable(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.transaction
   * RET:
   *   2  @return~ledger.table
   */
  struct ledger_transaction** a =
    (struct ledger_transaction**)luaL_checkudata
        (L, 1, ledger_llact_transaction_meta);
  struct ledger_table* t = ledger_transaction_get_table(*a);
  if (ledger_table_acquire(t) != t){
    luaL_error(L, "ledger.transaction.gettable: Table unavailable");
  } else {
    ledger_llbase_posttable
      (L, t, 1, "ledger.transaction.gettable: Table available");
  }
  return 1;
}

/* } END   ledger/act/transaction */

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
  /* add transaction lib */{
    luaL_newlib(L, ledger_luaL_transaction_lib);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llact_transaction_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llact_transaction_meta);
    }
    lua_setfield(L, -2, "transaction");
  }
  return;
}

/* END   implementation */
