
#include "luaext.h"
#include "../base/util.h"
#include "../../deps/lua/src/lua.h"
#include <stddef.h>
#include <string.h>

struct ledger_lua {
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
 * Read a string for Lua.
 * - base Lua state
 * - data string reader structure
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


/* BEGIN static implementation */

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

/* END   implementation */
