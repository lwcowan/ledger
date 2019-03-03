
#include "llbasetable.h"
#include "llbase.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lauxlib.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../base/table.h"
#include <limits.h>
#include <string.h>


static char const* ledger_llbase_table_meta = "ledger.table";
static char const* ledger_llbase_tablemark_meta = "ledger.table.mark";
static char const* ledger_llbase_bignum_meta = "ledger.bignum";

/*   BEGIN ledger/base/table { */

/*
 * `ledger.table.create()`
 * @return a table instance
 */
static int ledger_luaL_table_create(struct lua_State *L);

/*
 * `ledger.table.__gc(self~ledger.table)`
 * - self the number to free
 */
static int ledger_luaL_table___gc(struct lua_State *L);

/*
 * `ledger.table.__eq(a~ledger.table, b~ledger.table)`
 * - a first number
 * - b second number
 * @return true if equal, false otherwise
 */
static int ledger_luaL_table___eq(struct lua_State *L);

/*
 * `ledger.table.__len(self~ledger.table)`
 * - self the table to query
 * @return the number of rows in the table
 */
static int ledger_luaL_table___len(struct lua_State *L);

/*
 * `ledger.table.ptr(self~ledger.table)`
 * - self the table to query
 * @return the raw pointer to the table
 */
static int ledger_luaL_table_ptr(struct lua_State *L);

/*
 * `ledger.table.begin(self~ledger.table)`
 * - self the table to query
 * @return a mark pointing to the first row of the table
 */
static int ledger_luaL_table_begin(struct lua_State *L);

/*
 * `ledger.table.end(self~ledger.table)`
 * - self the table to query
 * @return a mark pointing to one after the last row of the table
 */
static int ledger_luaL_table_end(struct lua_State *L);

/*
 * `ledger.table.getcolumntypes(self~ledger.table)`
 * - self the table to query
 * @return a Lua table holding an array of type codes
 */
static int ledger_luaL_table_getcolumntypes(struct lua_State *L);

/*
 * `ledger.table.setcolumntypes(self~ledger.table, types~table)`
 * - self the table to query
 * - types array of type codes
 * @return a success flag
 */
static int ledger_luaL_table_setcolumntypes(struct lua_State *L);

/* [INTERNAL]
 * Set a Lua table to an integer array
 * - a pointer to array
 * - len number of integers in the array
 * - t Lua table
 */
static int ledger_luaL_table_setarrayP1(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_table_lib[] = {
  {"create", ledger_luaL_table_create},
  {"__gc", ledger_luaL_table___gc},
  {"__eq", ledger_luaL_table___eq},
  {"__len", ledger_luaL_table___len},
  {"ptr", ledger_luaL_table_ptr},
  {"beginmark", ledger_luaL_table_begin},
  {"endmark", ledger_luaL_table_end},
  {"getcolumntypes", ledger_luaL_table_getcolumntypes},
  {"setcolumntypes", ledger_luaL_table_setcolumntypes},
  {NULL,NULL}
};

struct ledger_luaL_table_enum {
  int value;
  char const* name;
};

static struct ledger_luaL_table_enum const ledger_luaL_table_types[] = {
  {LEDGER_TABLE_ID, "id"},
  {LEDGER_TABLE_BIGNUM, "bignum"},
  {LEDGER_TABLE_USTR, "ustr"},
  {LEDGER_TABLE_INDEX, "index"},
  {0,NULL}
};

/* } END   ledger/base/table */

/*   BEGIN ledger/base/table/mark { */

/*
 * `ledger.table.mark.__gc(self~ledger.table.mark)`
 * - self the mark to free
 */
static int ledger_luaL_tablemark___gc(struct lua_State *L);

/*
 * `ledger.table.mark.__eq(a~ledger.table.mark, b~ledger.table.mark)`
 * - a first mark
 * - b second mark
 * @return true if equal, false otherwise
 */
static int ledger_luaL_tablemark___eq(struct lua_State *L);

/*
 * `ledger.table.mark.move(left~ledger.table.mark, right~number)`
 * - left mark to move
 * - right number of rows by which to move the mark
 */
static int ledger_luaL_tablemark_move(struct lua_State *L);

/*
 * `ledger.table.mark.addrow(~ledger.table.mark)`
 * - self mark at which to add a row
 * @return a success flag
 */
static int ledger_luaL_tablemark_addrow(struct lua_State *L);

/*
 * `ledger.table.mark.droprow(~ledger.table.mark)`
 * - self mark at which to delete a row
 * @return a success flag
 */
static int ledger_luaL_tablemark_droprow(struct lua_State *L);

/*
 * `ledger.table.mark.__index(~ledger.table.mark, i~number)`
 * - self mark to query
 * - i column index
 * @return the object stored at that index
 */
static int ledger_luaL_tablemark_fetch(struct lua_State *L);

/*
 * `ledger.table.mark.__newindex(~ledger.table.mark, i~number, value)`
 * - self mark to query
 * - i column index
 * - value value to put
 * @return a success flag
 */
static int ledger_luaL_tablemark_put(struct lua_State *L);

static const struct luaL_Reg ledger_luaL_tablemark_lib[] = {
  {"__gc", ledger_luaL_tablemark___gc},
  {"__eq", ledger_luaL_tablemark___eq},
  {"move", ledger_luaL_tablemark_move},
  {"addrow", ledger_luaL_tablemark_addrow},
  {"droprow", ledger_luaL_tablemark_droprow},
  {"fetch", ledger_luaL_tablemark_fetch},
  {"put", ledger_luaL_tablemark_put},
  {NULL,NULL}
};

/* } END   ledger/base/table/mark */

/* BEGIN static implementation */

/*   BEGIN ledger/base/table { */

int ledger_luaL_table_create(struct lua_State *L){
  /* ARG:
   *   -
   * RET:
   *   1 @return
   * THROW:
   *   X
   */
  struct ledger_table* next_table;
  int ok;
  /* execute C API */{
    next_table = ledger_table_new();
    if (next_table != NULL){
      ok = 1;
    } else ok = 0;
  }
  ledger_llbase_posttable
    (L, next_table, ok, "ledger.table.create: generic error");
  return 1;
}

int ledger_luaL_table___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   * RET:
   *   X
   */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  ledger_table_free(*t);
  *t = NULL;
  return 0;
}

int ledger_luaL_table_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.table
   *   2  right~ledger.table
   * ...:
   */
  struct ledger_table** left =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  struct ledger_table** right =
    (struct ledger_table**)luaL_checkudata(L, 2, ledger_llbase_table_meta);
  if (*left != NULL && *right != NULL){
    return ledger_table_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_table_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_table___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.table
   *   2  right~ledger.table
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_table_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_table___len(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   * RET:
   *   2  @return~integer
   */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  int out_value = ledger_table_count_rows(*t);
  lua_pushinteger(L, (lua_Integer)out_value);
  return 1;
}

int ledger_luaL_table_ptr(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   * RET:
   *   2  @return~lightuserdata
   */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  lua_pushlightuserdata(L, (void*)*t);
  return 1;
}

int ledger_luaL_table_begin(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   * RET:
   *   2  @return~ledger.table.mark
   */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  struct ledger_table_mark *m = ledger_table_begin(*t);
  if (m != NULL){
    ledger_llbase_posttablemark(L, m, 1, "table mark allocation available");
  } else {
    luaL_error(L, "table mark allocation unavailable");
  }
  return 1;
}

int ledger_luaL_table_end(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   * RET:
   *   2  @return~ledger.table.mark
   */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  struct ledger_table_mark *m = ledger_table_end(*t);
  if (m != NULL){
    ledger_llbase_posttablemark(L, m, 1, "table mark allocation available");
  } else {
    luaL_error(L, "table mark allocation unavailable");
  }
  return 1;
}

int ledger_luaL_table_getcolumntypes(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   * RET:
   *   2  @return~table
   */
  /* get the table */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  /* query the needed array size */
  int array_size = ledger_table_get_column_count(*t);
  /* make the array */
  lua_createtable(L, array_size, 0);
  /* iterate over the types */{
    int i;
    for (i = 0; i < array_size; ++i){
      int typ;
      typ = ledger_table_get_column_type(*t, i);
      lua_pushinteger(L, typ);
      lua_seti(L, -2, (lua_Integer)(i+1));
    }
  }
  /* return */
  return 1;
}

int ledger_luaL_table_setarrayP1(struct lua_State *L){
  /* ARG:
   *   1  types~lightuserdata
   *   2  type_count~number
   *   3  ~table
   * RET:
   *   X
   */
  int *types = NULL;
  int type_count;
  int i;
  types = lua_touserdata(L, 1);
  type_count = lua_tointeger(L, 2);
  for (i = 0; i < type_count; ++i){
    /* get the table value */
    int value_type = lua_geti(L, 3, (lua_Integer)(i+1));
    switch (value_type){
    case LUA_TNUMBER:
      {
        types[i] = (int)lua_tointeger(L, -1);
      }break;
    default:
      {
        char const* text = lua_tostring(L, -1);
        int j;
        int subtype = 0;
        int const count =
          sizeof(ledger_luaL_table_types)/sizeof(*ledger_luaL_table_types);
        for (j = 0; j < count; ++j){
          if (ledger_luaL_table_types[j].name == NULL)
            break;
          if (strcmp(ledger_luaL_table_types[j].name, text) == 0){
            subtype = ledger_luaL_table_types[j].value;
            break;
          }
        }
        types[i] = subtype;
      }break;
    }
    lua_pop(L, 1);
  }
  return 0;
}

int ledger_luaL_table_setcolumntypes(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table
   *   2  ~table
   * RET:
   *   3  @return~boolean
   */
  int *types = NULL;
  int type_count;
  /* get the table */
  struct ledger_table** t =
    (struct ledger_table**)luaL_checkudata(L, 1, ledger_llbase_table_meta);
  /* construct the array */{
    lua_len(L, 2);
    type_count = lua_tointeger(L, -1);
    if (type_count > 0){
      types = ledger_util_malloc(type_count*sizeof(int));
      if (types == NULL){
        luaL_error(L, "ledger_luaL_table_setcolumntypes:"
            " unable to allocate transfer array");
      } else {
        int array_result;
        lua_pushcfunction(L, ledger_luaL_table_setarrayP1);
        lua_pushlightuserdata(L, types);
        lua_pushinteger(L, type_count);
        lua_pushvalue(L, 2);
        array_result = lua_pcall(L, 3, 0, 0);
        if (array_result != LUA_OK){
          ledger_util_free(types);
          lua_error(L);
          /*return 0;*/
        }
      }
    }
  }
  /* apply the array */{
    int apply_result;
    apply_result = ledger_table_set_column_types(*t, type_count, types);
    ledger_util_free(types);
    lua_pushboolean(L, apply_result?1:0);
  }
  return 1;
}

/* } END   ledger/base/table */

/*   BEGIN ledger/base/table/mark { */

int ledger_luaL_tablemark___gc(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table.mark
   * RET:
   *   X
   */
  struct ledger_table_mark** num = (struct ledger_table_mark**)luaL_checkudata
    (L, 1, ledger_llbase_tablemark_meta);
  ledger_table_mark_free(*num);
  *num = NULL;
  return 0;
}

int ledger_luaL_tablemark_cmp(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.table.mark
   *   2  right~ledger.table.mark
   * ...:
   */
  struct ledger_table_mark** left =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 1, ledger_llbase_tablemark_meta);
  struct ledger_table_mark** right =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 2, ledger_llbase_tablemark_meta);
  if (*left != NULL && *right != NULL){
    return ledger_table_mark_is_equal(*left, *right)?0:-1;
  } else {
    luaL_error(L, "ledger_luaL_tablemark_cmp: inconsistency check #1 triggered");
    return -2;
  }
}

int ledger_luaL_tablemark___eq(struct lua_State *L){
  /* ARG:
   *   1  left~ledger.table.mark
   *   2  right~ledger.table.mark
   * RET:
   *   3 @return~boolean
   */
  int result = ledger_luaL_tablemark_cmp(L);
  lua_pushboolean(L, result==0);
  return 1;
}

int ledger_luaL_tablemark_move(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table.mark
   *   2  ~number
   * RET:
   *   X
   */
  struct ledger_table_mark** mark =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 1, ledger_llbase_tablemark_meta);
  lua_Integer num = lua_tointeger(L, 2);
  ledger_table_mark_move(*mark, num);
  return 0;
}

int ledger_luaL_tablemark_addrow(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table.mark
   * RET:
   *   2 @return~boolean
   */
  int result;
  struct ledger_table_mark** mark =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 1, ledger_llbase_tablemark_meta);
  result = ledger_table_add_row(*mark);
  lua_pushboolean(L, result!=0);
  return 1;
}

int ledger_luaL_tablemark_droprow(struct lua_State *L){
  /* ARG:
   *   1  ~ledger.table.mark
   * RET:
   *   2 @return~boolean
   */
  int result;
  struct ledger_table_mark** mark =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 1, ledger_llbase_tablemark_meta);
  result = ledger_table_drop_row(*mark);
  lua_pushboolean(L, result!=0);
  return 1;
}

int ledger_luaL_tablemark_fetch(struct lua_State *L){
  /* ARG:
   *   1  mark~ledger.table.mark
   *   2  i~number
   * RET:
   *   3 @return~(number|ledger.bignum|string)
   */
  int result;
  int out_type;
  struct ledger_table_mark** mark =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 1, ledger_llbase_tablemark_meta);
  lua_Integer i = lua_tointeger(L, 2)-1;
  out_type = ledger_table_mark_get_type(*mark, (int)i);
  switch (out_type){
  case LEDGER_TABLE_ID:
    {
      int value;
      result = ledger_table_fetch_id(*mark, i, &value);
      if (!result){
        luaL_error(L, "ledger_luaL_tablemark_fetch: "
          "Identifier not available");
        break;
      }
      lua_pushinteger(L, (lua_Integer)value);
    }break;
  case LEDGER_TABLE_BIGNUM:
    {
      struct ledger_bignum* bn;
      bn = ledger_bignum_new();
      if (bn == NULL){
        luaL_error(L, "ledger_luaL_tablemark_fetch: "
          "Big number memory not available");
        break;
      }
      result = ledger_table_fetch_bignum(*mark, i, bn);
      ledger_llbase_postbignum
        (L, bn, result, "ledger_luaL_tablemark_fetch: "
          "Big number not available");
    }break;
  case LEDGER_TABLE_USTR:
    {
      unsigned char *buf;
      int length;
      length = ledger_table_fetch_string(*mark, i, NULL, 0);
      if (length < 0){
        luaL_error(L, "ledger_luaL_tablemark_fetch: "
          "String length not available");
        break;
      }
      buf = (unsigned char*)ledger_util_malloc(length+1);
      if (buf == NULL){
        luaL_error(L, "ledger_luaL_tablemark_fetch: "
          "String memory not available");
        break;
      }
      result = ledger_table_fetch_string(*mark, i, buf, length+1);
      ledger_llbase_poststring
        (L, buf, result>=0, "ledger_luaL_tablemark_fetch: "
          "String not available");
    }break;
  case LEDGER_TABLE_INDEX:
    {
      int value;
      result = ledger_table_fetch_id(*mark, i, &value);
      if (!result){
        luaL_error(L, "ledger_luaL_tablemark_fetch: "
          "Identifier not available");
        break;
      }
      /* ensure Lua one-index adjustment `+1` */
      lua_pushinteger(L, ((lua_Integer)value)+1);
    }break;
  default:
    lua_pushnil(L);
    break;
  }
  return 1;
}

int ledger_luaL_tablemark_put(struct lua_State *L){
  /* ARG:
   *   1  mark~ledger.table.mark
   *   2  i~number
   *   3  value~(number|ledger.bignum|string)
   * RET:
   *   4 @return~boolean
   */
  struct ledger_table_mark** mark =
    (struct ledger_table_mark**)luaL_checkudata
        (L, 1, ledger_llbase_tablemark_meta);
  lua_Integer i = lua_tointeger(L, 2)-1;
  int in_type;
  if (lua_isstring(L, 3)){
    in_type = LEDGER_TABLE_USTR;
  } else if (lua_isnumber(L, 3)){
    in_type = LEDGER_TABLE_ID;
  } else if (lua_isuserdata(L, 3)){
    if (luaL_testudata(L, 3, ledger_llbase_bignum_meta))
      in_type = LEDGER_TABLE_BIGNUM;
    else
      in_type = 0;
  } else in_type = 0;
  switch (in_type){
  case LEDGER_TABLE_ID:
    {
      int result;
      /* check if the destination is an index */{
        if (ledger_table_mark_get_type(*mark, i) == LEDGER_TABLE_INDEX){
          /* ensure Lua one-index adjustment `-1` */
          result = ledger_table_put_id(*mark, i, (int)(lua_tointeger(L, 3)-1));
          lua_pushboolean(L, result);
          break;
        }
      }
      result = ledger_table_put_id(*mark, i, (int)lua_tointeger(L, 3));
      lua_pushboolean(L, result);
    }break;
  case LEDGER_TABLE_BIGNUM:
    {
      int result;
      struct ledger_bignum** bnp =
          luaL_checkudata(L, 3, ledger_llbase_bignum_meta);
      /* check if the destination is an index */{
        if (ledger_table_mark_get_type(*mark, i) == LEDGER_TABLE_INDEX){
          /* ensure Lua one-index adjustment `-1` */
          result = ledger_table_put_id
            (*mark, i, (int)(ledger_bignum_get_long(*bnp)-1));
          lua_pushboolean(L, result);
          break;
        }
      }
      result = ledger_table_put_bignum(*mark, i, *bnp);
      lua_pushboolean(L, result);
    }break;
  case LEDGER_TABLE_USTR:
    {
      int result;
      char const* string = lua_tostring(L, 3);
      /* check if the destination is an index */{
        if (ledger_table_mark_get_type(*mark, i) == LEDGER_TABLE_INDEX){
          /* ensure Lua one-index adjustment `-1` */
          result = ledger_table_put_id
            (*mark, i, (ledger_util_atoi((unsigned char const*)string)-1));
          lua_pushboolean(L, result);
          break;
        }
      }
      result = ledger_table_put_string
          (*mark, i, (unsigned char const*)string);
      lua_pushboolean(L, result);
    }break;
  default:
    lua_pushboolean(L, 0);
    break;
  }
  return 1;
}

/* } END   ledger/base/table/mark */

/* END   static implementation */

/* BEGIN implementation */

void ledger_luaopen_basetable(struct lua_State *L){
  /* add table lib */{
    luaL_newlib(L, ledger_luaL_table_lib);
    /* add table mark lib */{
      luaL_newlib(L, ledger_luaL_tablemark_lib);
      lua_pushvalue(L, -1);
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
      lua_pushstring(L, ledger_llbase_table_meta);
      lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
      /* registry.name = metatable */{
        lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_tablemark_meta);
      }
      lua_setfield(L, -2, "mark");
    }
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
    lua_pushstring(L, ledger_llbase_table_meta);
    lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    /* registry.name = metatable */{
      lua_setfield(L, LUA_REGISTRYINDEX, ledger_llbase_table_meta);
    }
    lua_setfield(L, -2, "table");
  }
  return;
}

/* END   implementation */

