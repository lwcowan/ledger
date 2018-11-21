
#include "../src/base/bignum.h"
#include "../src/base/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static int allocate_test(void);
static int trivial_compare_test(void);
static int set_long_test(void);
static int set_limit_long_test(void);
static int internal_allocate_test(void);
static int get_text_test(void);
static int set_text_test(void);
static int number_swap_test(void);
static int nonzero_compare_test(void);
static int number_overprimed_copy_test(void);
static int number_primed_copy_test(void);
static int number_copy_test(void);
static int number_unprimed_copy_test(void);
static int negate_test(void);
static int add_test(void);
static int add_inverted_test(void);
static int add_carry_test(void);
static int subtract_test(void);
static int subtract_inverted_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { trivial_compare_test, "trivial compare" },
  { internal_allocate_test, "internal allocate" },
  { set_long_test, "set long" },
  { set_limit_long_test, "set limit long" },
  { get_text_test, "get text" },
  { set_text_test, "set text" },
  { number_swap_test, "number swap" },
  { number_unprimed_copy_test, "number copy: unprimed" },
  { number_primed_copy_test, "number copy: underprimed" },
  { number_copy_test, "number copy: primed" },
  { number_overprimed_copy_test, "number copy: overprimed" },
  { nonzero_compare_test, "nonzero compare" },
  { negate_test, "negate" },
  { add_test, "add" },
  { add_carry_test, "add with carry" },
  { add_inverted_test, "add inverted" },
  { subtract_test, "subtract" },
  { subtract_inverted_test, "subtract inverted" }
};


int allocate_test(void){
  struct ledger_bignum* ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  ledger_bignum_free(ptr);
  return 1;
}

int internal_allocate_test(void){
  int result = 0;
  struct ledger_bignum* ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  do {
    int ok;
    if (ledger_bignum_count_digits(ptr) != 0) break;
    if (ledger_bignum_find_point(ptr) != 0) break;
    /* invalid allocation */
    ok = ledger_bignum_alloc(ptr,4,5);
    if (ok) break;
    if (ledger_bignum_count_digits(ptr) != 0) break;
    if (ledger_bignum_find_point(ptr) != 0) break;
    /* valid allocation */
    ok = ledger_bignum_alloc(ptr,4,2);
    if (!ok) break;
    if (ledger_bignum_count_digits(ptr) != 4) break;
    if (ledger_bignum_find_point(ptr) != 2) break;
    result = 1;
  } while (0);
  ledger_bignum_free(ptr);
  return result;
}

int trivial_compare_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    if (ledger_bignum_compare(ptr,other_ptr) != 0) break;
    if (ledger_bignum_compare(other_ptr, ptr) != 0) break;
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}

int set_long_test(void){
  int result = 0;
  struct ledger_bignum* ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  do {
    if (!ledger_bignum_set_long(ptr,378)) break;
    if (ledger_bignum_get_long(ptr) != 378) break;
    if (!ledger_bignum_set_long(ptr,-96058l)) break;
    if (ledger_bignum_get_long(ptr) != -96058l) break;
    if (!ledger_bignum_set_long(ptr,392050493l)) break;
    if (ledger_bignum_get_long(ptr) != 392050493l) break;
    if (!ledger_bignum_set_long(ptr,-2092050493l)) break;
    if (ledger_bignum_get_long(ptr) != -2092050493l) break;
    result = 1;
  } while (0);
  ledger_bignum_free(ptr);
  return result;
}

int set_limit_long_test(void){
  int result = 0;
  struct ledger_bignum* ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  do {
    if (!ledger_bignum_set_long(ptr,LONG_MIN)) break;
    if (ledger_bignum_get_long(ptr) != LONG_MIN) break;
    if (!ledger_bignum_set_long(ptr,LONG_MIN+1)) break;
    if (ledger_bignum_get_long(ptr) != LONG_MIN+1) break;
    if (!ledger_bignum_set_long(ptr,LONG_MAX)) break;
    if (ledger_bignum_get_long(ptr) != LONG_MAX) break;
    if (!ledger_bignum_set_long(ptr,LONG_MAX-1)) break;
    if (ledger_bignum_get_long(ptr) != LONG_MAX-1) break;
    result = 1;
  } while (0);
  ledger_bignum_free(ptr);
  return result;
}

int get_text_test(void){
  int result = 0;
  struct ledger_bignum* ptr;
  unsigned char buffer[16];
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  do {
    if (!ledger_bignum_set_long(ptr,-378)) break;
    if (ledger_bignum_get_long(ptr) != -378) break;
    if (ledger_bignum_get_text(ptr,NULL,0,0) != 4) break;
    if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),0) != 4) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)"-378",buffer) != 0) break;
    if (!ledger_bignum_alloc(ptr,8,4)) break;
    if (!ledger_bignum_set_long(ptr,9515)) break;
    if (ledger_bignum_get_text(ptr,NULL,0,0) != 13) break;
    if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),0) != 13) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)"9515.00000000",buffer) != 0) break;
    if (ledger_bignum_get_text(ptr,NULL,0,1) != 14) break;
    if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 14) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)"+9515.00000000",buffer) != 0) break;
    result = 1;
  } while (0);
  ledger_bignum_free(ptr);
  return result;
}

int set_text_test(void){
  int result = 0;
  struct ledger_bignum* ptr;
  unsigned char buffer[24];
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  do {
    if (!ledger_bignum_set_text(
        ptr,(unsigned char const*)"-378",NULL)) break;
    if (ledger_bignum_get_long(ptr) != -378) break;
    if (!ledger_bignum_set_text(
        ptr,(unsigned char const*)"-378.",NULL)) break;
    if (ledger_bignum_get_long(ptr) != -378) break;
    if (!ledger_bignum_set_text(
        ptr,(unsigned char const*)"-00000378.0",NULL)) break;
    if (ledger_bignum_get_long(ptr) != -378) break;
    if (ledger_bignum_get_text(ptr,NULL,0,0) != 7) break;
    if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),0) != 7) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)"-378.00",buffer) != 0) break;
    if (!ledger_bignum_alloc(ptr,8,4)) break;
    /* endptr check */{
      unsigned char const* text9515 = (unsigned char const*)"9515";
      unsigned char* text_ptr;
      if (!ledger_bignum_set_text(
          ptr,text9515,&text_ptr)) break;
      if (text_ptr != text9515+4) break;
    }
    if (ledger_bignum_get_text(ptr,NULL,0,0) != 13) break;
    if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),0) != 13) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)"9515.00000000",buffer) != 0) break;
    if (ledger_bignum_get_text(ptr,NULL,0,1) != 14) break;
    if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 14) break;
    if (ledger_util_ustrcmp(
        (unsigned char const*)"+9515.00000000",buffer) != 0) break;
    if (ledger_bignum_get_long(ptr) != +9515) break;
    result = 1;
  } while (0);
  ledger_bignum_free(ptr);
  return result;
}

int number_swap_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    if (!ledger_bignum_set_long(ptr, -43)) break;
    if (!ledger_bignum_set_long(other_ptr, +45)) break;
    if (ledger_bignum_get_long(ptr) != -43) break;
    if (ledger_bignum_get_long(other_ptr) != +45) break;
    ledger_bignum_swap(ptr, other_ptr);
    if (ledger_bignum_get_long(ptr) != +45) break;
    if (ledger_bignum_get_long(other_ptr) != -43) break;
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}

int nonzero_compare_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    if (!ledger_bignum_alloc(ptr,4,2)) break;
    if (!ledger_bignum_alloc(other_ptr,5,1)) break;
    if (ledger_bignum_compare(ptr,other_ptr) != 0) break;
    if (ledger_bignum_compare(other_ptr, ptr) != 0) break;
    if (!ledger_bignum_set_text(
      ptr,(unsigned char const*)"48.01",NULL)) break;
    if (!ledger_bignum_set_long(other_ptr,48)) break;
    if (ledger_bignum_compare(other_ptr, ptr) >= 0) break;
    if (ledger_bignum_compare(ptr, other_ptr) <= 0) break;
    if (!ledger_bignum_set_text(
      ptr,(unsigned char const*)"46",NULL)) break;
    if (ledger_bignum_compare(ptr, other_ptr) >= 0) break;
    if (ledger_bignum_compare(other_ptr, ptr) <= 0) break;
    if (!ledger_bignum_set_text(
      ptr,(unsigned char const*)"48",NULL)) break;
    if (ledger_bignum_compare(ptr, other_ptr) != 0) break;
    if (ledger_bignum_compare(other_ptr, ptr) != 0) break;
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}

int number_overprimed_copy_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    unsigned char const *numeric = (unsigned char const*)"-2345.98";
    if (!ledger_bignum_set_text(other_ptr, numeric, NULL)) break;
    if (ledger_bignum_get_long(other_ptr) != -2345) break;
    /* test the overprimed truncate */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 5, 2);
      if (!ok) break;
      ledger_bignum_truncate(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 10) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.9800",buffer) != 0) break;
    }
    /* test the overprimed assign */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 5, 2);
      if (!ok) break;
      ledger_bignum_assign(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 10) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.9800",buffer) != 0) break;
    }
    /* test the overprimed copy */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 5, 2);
      if (!ok) break;
      ledger_bignum_copy(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}

int number_copy_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    unsigned char const *numeric = (unsigned char const*)"-2345.98";
    if (!ledger_bignum_set_text(other_ptr, numeric, NULL)) break;
    if (ledger_bignum_get_long(other_ptr) != -2345) break;
    /* test the primed truncate */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 3, 1);
      if (!ok) break;
      ledger_bignum_truncate(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    /* test the primed assign */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 3, 1);
      if (!ok) break;
      ledger_bignum_assign(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    /* test the primed copy */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 3, 1);
      if (!ok) break;
      ledger_bignum_copy(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}


int number_unprimed_copy_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    unsigned char const *numeric = (unsigned char const*)"-2345.98";
    if (!ledger_bignum_set_text(other_ptr, numeric, NULL)) break;
    if (ledger_bignum_get_long(other_ptr) != -2345) break;
    /* test the unprimed truncate */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 0, 0);
      if (!ok) break;
      ledger_bignum_truncate(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 2) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-0",buffer) != 0) break;
    }
    /* test the unprimed assign */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 0, 0);
      if (!ok) break;
      ledger_bignum_assign(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    /* test the unprimed copy */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 0, 0);
      if (!ok) break;
      ledger_bignum_copy(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}

int number_primed_copy_test(void){
  int result = 0;
  struct ledger_bignum* ptr, * other_ptr;
  ptr = ledger_bignum_new();
  if (ptr == NULL) return 0;
  other_ptr = ledger_bignum_new();
  if (other_ptr == NULL){
    ledger_bignum_free(ptr);
    return 0;
  }
  do {
    unsigned char const *numeric = (unsigned char const*)"-2345.98";
    if (!ledger_bignum_set_text(other_ptr, numeric, NULL)) break;
    if (ledger_bignum_get_long(other_ptr) != -2345) break;
    /* test the primed truncate */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 1, 0);
      if (!ok) break;
      ledger_bignum_truncate(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 3) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-45",buffer) != 0) break;
    }
    /* test the primed assign */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 1, 0);
      if (!ok) break;
      ledger_bignum_assign(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    /* test the primed copy */{
      unsigned char buffer[16];
      int ok;
      ok = ledger_bignum_alloc(ptr, 1, 0);
      if (!ok) break;
      ledger_bignum_copy(ptr, other_ptr);
      if (ledger_bignum_get_text(ptr,buffer,sizeof(buffer),1) != 8) break;
      if (ledger_util_ustrcmp(
          (unsigned char const*)"-2345.98",buffer) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_bignum_free(other_ptr);
  ledger_bignum_free(ptr);
  return result;
}

int negate_test(void){
  int result = 0;
  struct ledger_bignum* a, * b, * c;
  a = ledger_bignum_new();
  if (a == NULL) return 0;
  b = ledger_bignum_new();
  if (b == NULL){
    ledger_bignum_free(a);
    return 0;
  }
  c = ledger_bignum_new();
  if (c == NULL){
    ledger_bignum_free(a);
    ledger_bignum_free(b);
    return 0;
  }
  do {
    unsigned char buf[16];
    if (!ledger_bignum_set_text
        (a,(unsigned char const*)"-99.45",NULL))
      break;
    if (!ledger_bignum_negate(b,a)) break;
    if (ledger_bignum_get_text(b,buf,sizeof(buf),0) != 5) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"99.45") != 0)
      break;
    if (!ledger_bignum_negate(c,b)) break;
    if (ledger_bignum_get_text(c,buf,sizeof(buf),0) != 6) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"-99.45") != 0)
      break;
    if (ledger_bignum_compare(a,c) != 0) break;
    result = 1;
  } while (0);
  ledger_bignum_free(c);
  ledger_bignum_free(b);
  ledger_bignum_free(a);
  return result;
}

int add_test(void){
  int result = 0;
  struct ledger_bignum* a, * b, * c;
  a = ledger_bignum_new();
  if (a == NULL) return 0;
  b = ledger_bignum_new();
  if (b == NULL){
    ledger_bignum_free(a);
    return 0;
  }
  c = ledger_bignum_new();
  if (c == NULL){
    ledger_bignum_free(a);
    ledger_bignum_free(b);
    return 0;
  }
  do {
    unsigned char buf[16];
    if (!ledger_bignum_set_text
        (a,(unsigned char const*)"-93.45",NULL))
      break;
    if (!ledger_bignum_set_text
        (b,(unsigned char const*)"-120.781",NULL))
      break;
    if (!ledger_bignum_add(c,a,b)) break;
    if (ledger_bignum_get_text(c,buf,sizeof(buf),0) != 9) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"-214.2310") != 0)
      break;
    result = 1;
  } while (0);
  ledger_bignum_free(c);
  ledger_bignum_free(b);
  ledger_bignum_free(a);
  return result;
}

int add_inverted_test(void){
  int result = 0;
  struct ledger_bignum* a, * b, * c;
  a = ledger_bignum_new();
  if (a == NULL) return 0;
  b = ledger_bignum_new();
  if (b == NULL){
    ledger_bignum_free(a);
    return 0;
  }
  c = ledger_bignum_new();
  if (c == NULL){
    ledger_bignum_free(a);
    ledger_bignum_free(b);
    return 0;
  }
  do {
    unsigned char buf[16];
    if (!ledger_bignum_set_text
        (a,(unsigned char const*)"-93.45",NULL))
      break;
    if (!ledger_bignum_set_text
        (b,(unsigned char const*)"+120.781",NULL))
      break;
    if (!ledger_bignum_add(c,a,b)) break;
    if (ledger_bignum_get_text(c,buf,sizeof(buf),0) != 7) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"27.3310") != 0)
      break;
    result = 1;
  } while (0);
  ledger_bignum_free(c);
  ledger_bignum_free(b);
  ledger_bignum_free(a);
  return result;
}

int subtract_test(void){
  int result = 0;
  struct ledger_bignum* a, * b, * c;
  a = ledger_bignum_new();
  if (a == NULL) return 0;
  b = ledger_bignum_new();
  if (b == NULL){
    ledger_bignum_free(a);
    return 0;
  }
  c = ledger_bignum_new();
  if (c == NULL){
    ledger_bignum_free(a);
    ledger_bignum_free(b);
    return 0;
  }
  do {
    unsigned char buf[16];
    if (!ledger_bignum_set_text
        (a,(unsigned char const*)"-93.45",NULL))
      break;
    if (!ledger_bignum_set_text
        (b,(unsigned char const*)"-120.781",NULL))
      break;
    if (!ledger_bignum_subtract(c,a,b)) break;
    if (ledger_bignum_get_text(c,buf,sizeof(buf),0) != 7) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"27.3310") != 0)
      break;
    result = 1;
  } while (0);
  ledger_bignum_free(c);
  ledger_bignum_free(b);
  ledger_bignum_free(a);
  return result;
}

int subtract_inverted_test(void){
  int result = 0;
  struct ledger_bignum* a, * b, * c;
  a = ledger_bignum_new();
  if (a == NULL) return 0;
  b = ledger_bignum_new();
  if (b == NULL){
    ledger_bignum_free(a);
    return 0;
  }
  c = ledger_bignum_new();
  if (c == NULL){
    ledger_bignum_free(a);
    ledger_bignum_free(b);
    return 0;
  }
  do {
    unsigned char buf[16];
    if (!ledger_bignum_set_text
        (a,(unsigned char const*)"-93.45",NULL))
      break;
    if (!ledger_bignum_set_text
        (b,(unsigned char const*)"+120.781",NULL))
      break;
    if (!ledger_bignum_subtract(c,a,b)) break;
    if (ledger_bignum_get_text(c,buf,sizeof(buf),0) != 9) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"-214.2310") != 0)
      break;
    result = 1;
  } while (0);
  ledger_bignum_free(c);
  ledger_bignum_free(b);
  ledger_bignum_free(a);
  return result;
}

int add_carry_test(void){
  int result = 0;
  struct ledger_bignum* a, * b, * c;
  a = ledger_bignum_new();
  if (a == NULL) return 0;
  b = ledger_bignum_new();
  if (b == NULL){
    ledger_bignum_free(a);
    return 0;
  }
  c = ledger_bignum_new();
  if (c == NULL){
    ledger_bignum_free(a);
    ledger_bignum_free(b);
    return 0;
  }
  do {
    unsigned char buf[16];
    if (!ledger_bignum_set_text
        (a,(unsigned char const*)"93.45",NULL))
      break;
    if (!ledger_bignum_set_text
        (b,(unsigned char const*)"20.78",NULL))
      break;
    if (!ledger_bignum_add(c,a,b)) break;
    if (ledger_bignum_get_text(c,buf,sizeof(buf),0) != 6) break;
    if (ledger_util_ustrcmp(buf,
        (unsigned char const*)"114.23") != 0)
      break;
    result = 1;
  } while (0);
  ledger_bignum_free(c);
  ledger_bignum_free(b);
  ledger_bignum_free(a);
  return result;
}



int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)())?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
