
#include "luaext.h"
#include "../base/util.h"
#include "../../deps/lua/src/lua.h"
#include <stddef.h>
#include <string.h>

struct ledger_lua {
  lua_State* base;
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

/* END   implementation */
