
#include "luaext.h"
#include "../base/util.h"
#include "../../deps/lua/src/lua.h"
#include "../../deps/lua/src/lualib.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

struct ledger_lua {
  int lib_type;
  lua_State* base;
};

/*
 * String reader structure.
 */
struct ledger_lua_str_reader {
  /* Text to read. */
  unsigned char const* p;
};

/*
 * File reader structure.
 */
struct ledger_lua_file_reader {
  /* File to read. */
  FILE* f;
  /* Error encounter. */
  int err;
  /* read buffer */
  unsigned char buf[256];
};


/*
 * Close a Lua structure.
 * - l the Lua structure to close
 */
void ledger_lua_clear(struct ledger_lua* l);

/*
 * Initialize a Lua structure.
 * - l the Lua structure to initialize
 * @return one on success, zero otherwise
 */
int ledger_lua_init(struct ledger_lua* l);

/*
 * Allocate and free memory for the Lua state.
 * - ud user data
 * - ptr pointer to memory block to manage
 * - osize old block size when `ptr` is not NULL, otherwise
 *     a type code
 * - nsize new desired block size
 * @return a pointer to allocated memory on successful allocation,
 *   NULL on successful free, or NULL otherwise
 */
void* ledger_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize);

/*
 * Load a text string of a certain length.
 * - l ledger-lua state
 * - ss NUL-terminated string to load
 * @return one on success, zero otherwise
 */
static int ledger_lua_load_str
  ( struct ledger_lua const* l, unsigned char const* ss,
    unsigned char const* chunk_name);

/*
 * Load a text string of a certain length.
 * - l ledger-lua state
 * - fn NUL-terminated name of file to load
 * @return one on success, zero otherwise
 */
static int ledger_lua_load_file
  ( struct ledger_lua const* l, char const* fn,
    unsigned char const* chunk_name);

/*
 * Read a string for Lua.
 * - base Lua state
 * - data string reader structure
 * - size pointer to hold output size
 * @return the next chunk for processing on success, or NULL at end-of-string
 */
static char const* ledger_lua_str_read
  (lua_State *base, void *data, size_t *size);

/*
 * Read a file for Lua.
 * - base Lua state
 * - data file reader structure
 * - size pointer to hold output size
 * @return the next chunk for processing on success, or NULL at end-of-string
 */
static char const* ledger_lua_str_read
  (lua_State *base, void *data, size_t *size);

/*
 * Run the function at the top of the stack.
 * - l ledger-lua state
 * @return one on success, zero otherwise
 */
static int ledger_lua_exec_top( struct ledger_lua* l);

/*
 * from the default Lua 5.3.5 interpreter:
 *
 * Create the 'arg' table, which stores all arguments from the
 * command line ('argv'). It should be aligned so that, at index 0,
 * it has 'argv[script]', which is the script name. The arguments
 * to the script (everything after 'script') go to positive indices;
 * other arguments (before the script name) go to negative indices.
 * If there is no script name, assume interpreter's name as base.
 */
static int ledger_lua_set_arg_i(lua_State *L);



/* BEGIN static implementation */

int ledger_lua_set_arg_i(lua_State *L){
  /*
   * Code based on default Lua 5.3.5 interpreter
   * Copyright (C) 1994-2018 Lua.org, PUC-Rio.
   * Licensed under the MIT License.
   */
  char **argv = (char **)lua_touserdata(L, 2);
  int argc = (int)lua_tointeger(L, 1);
  int script = (int)lua_tointeger(L, 3);
  int i, narg;
  if (script == argc) script = 0;  /* no script name? */
  narg = argc - (script + 1);  /* number of positive indices */
  lua_createtable(L, narg, script + 1);
  for (i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i - script);
  }
  lua_setglobal(L, "arg");
  return 0;
}

void ledger_lua_clear(struct ledger_lua* l){
  if (l->base != NULL){
    lua_close(l->base);
    l->base = NULL;
  }
  return;
}

int ledger_lua_init(struct ledger_lua* l){
  lua_State *new_state = lua_newstate(&ledger_lua_alloc, l);
  if (new_state == NULL){
    return 0;
  }
  l->base = new_state;
  l->lib_type = 0;
  return 1;
}

void* ledger_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize){
  struct ledger_lua* const l = (struct ledger_lua*)ud;
  (void)l;
  if (nsize == 0){
    ledger_util_free(ptr);
    return NULL;
  } else if (ptr != NULL){
    if (nsize == osize) return ptr;
    else if (nsize < osize){
      void* new_ptr = ledger_util_malloc(nsize);
      if (new_ptr != NULL){
        memcpy(new_ptr, ptr, nsize);
        ledger_util_free(ptr);
        return new_ptr;
      } else return new_ptr;
    } else {
      void* new_ptr = ledger_util_malloc(nsize);
      if (new_ptr != NULL){
        memcpy(new_ptr, ptr, osize);
        ledger_util_free(ptr);
        return new_ptr;
      } else return NULL;
    }
  } else switch (osize){
  case LUA_TSTRING:
    if (nsize >= 65534){
      /* artificial limit for strings */
      return NULL;
    } else return ledger_util_malloc(nsize);
  default:
    return ledger_util_malloc(nsize);
  }
}

const char * ledger_lua_str_read(lua_State *l, void *data, size_t *size){
  struct ledger_lua_str_reader *const reader =
      (struct ledger_lua_str_reader *)data;
  if (reader->p[0] != 0){
    char const* out = (char const*)(reader->p);
    size_t i;
    for (i = 0; i < 256 && reader->p[0] != 0; ++i){
      reader->p += 1;
    }
    *size = i;
    return out;
  } else {
    *size = 0;
    return NULL;
  }
}

const char * ledger_lua_file_read(lua_State *l, void *data, size_t *size){
  struct ledger_lua_file_reader *const reader =
      (struct ledger_lua_file_reader *)data;
  size_t len = fread
    (reader->buf,sizeof(unsigned char), sizeof(reader->buf), reader->f);
  if (ferror(reader->f)){
    *size = 0;
    reader->err = 1;
    return NULL;
  } else if (len > 0){
    *size = len;
    return (char const*)reader->buf;
  } else {
    *size = 0;
    return NULL;
  }
}

int ledger_lua_load_str
  ( struct ledger_lua const* l, unsigned char const* ss,
    unsigned char const* chunk_name)
{
  int l_result;
  struct ledger_lua_str_reader reader = { ss };
  l_result = lua_load(l->base,
      &ledger_lua_str_read, &reader, (char const*)chunk_name, "t");
  return l_result==LUA_OK?1:0;
}

int ledger_lua_load_file
  ( struct ledger_lua const* l, char const* fn,
    unsigned char const* chunk_name)
{
  int l_result;
  FILE *f = fopen(fn,"rt");
  if (f != NULL){
    struct ledger_lua_file_reader reader = { f, 0 };
    l_result = lua_load(l->base,
        &ledger_lua_file_read, &reader, (char const*)chunk_name, "t");
    fclose(f);
    if (l_result == LUA_OK && reader.err){
      /* reject successful partial compile */
      lua_pop(l->base, 1);
      return 0;
    } else return l_result==LUA_OK?1:0;
  } else return 0;
}

int ledger_lua_exec_top(struct ledger_lua* l){
  int l_result;
  if (!lua_isfunction(l->base, -1))
    return 0;
  else {
    l_result = lua_pcall(l->base, 0, 0, 0);
    return l_result==LUA_OK?1:0;
  }
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_lua* ledger_lua_new(void){
  struct ledger_lua* a = (struct ledger_lua* )ledger_util_malloc
    (sizeof(struct ledger_lua));
  if (a != NULL){
    if (!ledger_lua_init(a)){
      ledger_util_free(a);
      a = NULL;
    }
  }
  return a;
}

void ledger_lua_close(struct ledger_lua* a){
  if (a != NULL){
    ledger_lua_clear(a);
    ledger_util_free(a);
  }
}

int ledger_lua_exec_str
  (struct ledger_lua* l, unsigned char const* name, unsigned char const* s)
{
  int result;
  result = ledger_lua_load_str(l,s, name);
  if (result == 0) return 0;
  else return ledger_lua_exec_top(l);
}

int ledger_lua_openlibs(struct ledger_lua* l){
  int ok = 0;
  if (l->lib_type != 0){
    return l->lib_type == 1;
  } else do {
    luaL_openlibs(l->base);
    ok = 1;
  } while (0);
  if (ok) l->lib_type = 1;
  else l->lib_type = -1;
  return ok;
}

int ledger_lua_exec_file(struct ledger_lua* l,
    unsigned char const* name, char const* f)
{
  int result;
  result = ledger_lua_load_file(l,f, name);
  if (result == 0) return 0;
  else return ledger_lua_exec_top(l);
}

int ledger_lua_set_arg
  (struct ledger_lua* l, char **argv, int argc, int script)
{
  int status;
  lua_State *ls = l->base;
  /* use protected call technique from default Lua interpreter */
  /*
   * Code based on default Lua 5.3.5 interpreter
   * Copyright (C) 1994-2018 Lua.org, PUC-Rio.
   * Licensed under the MIT License.
   */
  lua_pushcfunction(ls, &ledger_lua_set_arg_i);
    /* to call '...set_arg_i' in protected mode */
  lua_pushinteger(ls, argc);  /* 1st argument */
  lua_pushlightuserdata(ls, argv); /* 2nd argument */
  lua_pushinteger(ls, script);  /* 3rd argument */
  status = lua_pcall(ls, 3, 0, 0);  /* do the call */
  return status==LUA_OK?1:0;
}

/* END   implementation */
