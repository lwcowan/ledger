
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

unsigned char* ledger_util_ustrndup
  (unsigned char const* str, size_t sz, int* ok)
{
  if (str != NULL){
    size_t len = sz;
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


int ledger_util_ustrcmp(unsigned char const* a, unsigned char const* b){
  /* trivial comparisons */
  if (a == NULL && b == NULL) return 0;
  else if (a == NULL) return -1;
  else if (b == NULL) return +1;
  /* bytewise comparisons */
  while ((*a != 0) && (*b != 0)){
    if (*a != *b) break;
    ++a;
    ++b;
  }
  if (*a == *b) return 0;
  else if (*a < *b) return -1;
  else return +1;
}

int ledger_util_uiszero(unsigned char const* a, size_t siz){
  size_t i;
  for (i = 0; i < siz; ++i){
    if (a[i] != 0) return 0;
  }
  return 1;
}

size_t ledger_util_itoa(int n, unsigned char* buf, size_t siz, int want_plus){
  size_t write_point = 0;
  unsigned char internal_buffer[sizeof(int)*((CHAR_BIT+2)/3)+1];
  if (want_plus || n < 0){
    if (write_point < siz) buf[write_point] = (n<0)?'-':'+';
    write_point += 1;
  }
  if (n != 0){
    unsigned int mutable_n = (unsigned int)abs(n);
    int i = 0;
    /* place digits to internal buffer */
    while (mutable_n > 0){
      internal_buffer[i] = (mutable_n % 10)+'0';
      i += 1;
      mutable_n /= 10;
    }
    /* move digits to external buffer */
    for (; i > 0; --i){
      if (write_point < siz) buf[write_point] = internal_buffer[i-1];
      write_point += 1;
    }
  } else {
    if (write_point < siz) buf[write_point] = '0';
    write_point += 1;
  }
  if (write_point < siz) buf[write_point] = 0;
  else if (siz > 0) buf[siz-1] = 0;
  return write_point;
}

int ledger_util_atoi(unsigned char const* str){
  return atoi((char const*)str);
}

/* END   implementation */

