
#include "util.h"
#include <stdlib.h>
#include <string.h>


/* BEGIN implementation */

void* ledger_util_malloc(size_t siz){
  if (siz == 0) return NULL;
  return malloc(siz);
}

void ledger_util_free(void* ptr){
  free(ptr);
  return;
}

size_t ledger_util_ustrlen(unsigned char const* str){
  size_t out = 0;
  while (*str != 0){
    out += 1;
    ++str;
  }
  return out;
}

unsigned char* ledger_util_ustrdup(unsigned char const* str, int* ok){
  if (str != NULL){
    size_t len = ledger_util_ustrlen(str);
    unsigned char* ptr;
    if (len >= 65534){
      /* string too long */
      if (ok) *ok = 0;
      return NULL;
    } else {
      ptr = ledger_util_malloc(len+1);
      if (ptr != NULL){
        memcpy(ptr,str,len);
        ptr[len] = 0;
        if (ok) *ok = 1;
      } else {
        if (ok) *ok = 0;
      }
      return ptr;
    }
  } else {
    if (ok) *ok = 1;
    return NULL;
  }
}

/* END   implementation */

