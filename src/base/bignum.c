
#include "bignum.h"
#include "util.h"
#include <limits.h>
#include <string.h>

/*
 * sanity check on digit count
 */
#ifndef LEDGER_BIGNUM_DIGIT_MAX
#  define LEDGER_BIGNUM_DIGIT_MAX 100
#endif /*LEDGER_BIGNUM_DIGIT_MAX*/
#if LEDGER_BIGNUM_DIGIT_MAX > (INT_MAX/2-4)
#  error "LEDGER_BIGNUM_DIGIT_MAX too large"
#endif /*LEDGER_BIGNUM_DIGIT_MAX*/

/*
 * Actualization of the big number structure
 */
struct ledger_bignum {
  /* base-100 digits, stored in little-endian order */
  unsigned char *digits;
  /* number of digits, equal to the number of bytes */
  int digit_count;
  /* position of the centesimal point, in digits */
  int point_place;
  /* one if the number is negative, zero otherwise */
  char negative;
};

/*
 * Initialize a number.
 * - n number to initialize
 * @return one on success, zero on failure
 */
static int ledger_bignum_init(struct ledger_bignum* n);
/*
 * Clear out a number.
 * - n number to clear
 */
static void ledger_bignum_clear(struct ledger_bignum* n);


/* BEGIN static implementation */

int ledger_bignum_init(struct ledger_bignum* n){
  n->digits = NULL;
  n->digit_count = 0;
  n->point_place = 0;
  n->negative = 0;
  return 1;
}

void ledger_bignum_clear(struct ledger_bignum* n){
  ledger_util_free(n->digits);
  n->digits = NULL;
  n->digit_count = 0;
  n->point_place = 0;
  n->negative = 0;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_bignum* ledger_bignum_new(void){
  struct ledger_bignum* n = (struct ledger_bignum* )ledger_util_malloc
    (sizeof(struct ledger_bignum));
  if (n != NULL){
    if (!ledger_bignum_init(n)){
      ledger_util_free(n);
      n = NULL;
    }
  }
  return n;
}

void ledger_bignum_free(struct ledger_bignum* n){
  if (n != NULL){
    ledger_bignum_clear(n);
    ledger_util_free(n);
  }
}

int ledger_bignum_compare
  (struct ledger_bignum const* a, struct ledger_bignum const* b)
{
  return 0;
}

int ledger_bignum_set_long(struct ledger_bignum* n, long int v){
  if (v == 0){
    if (n->digits != NULL)
      memset(n->digits,0,sizeof(unsigned char)*(n->digit_count));
    n->negative = 0;
    return 1;
  } else /* compute space needed to store the number */{
    unsigned long int nv;
    int neg;
    int digit_count = 0;
    /* convert to unsigned */
    if (v < 0){
      neg = 1;
      nv = (unsigned long int)-v;
    } else {
      neg = 0;
      nv = (unsigned long int)+v;
    }
    /* count the digits */{
      unsigned long int modal_nv = nv;
      while (modal_nv > 0){
        modal_nv /= 100;
        digit_count += 1;
      }
    }
    /* allocate if needed */{
      if (n->digit_count - n->point_place < digit_count){
        int const ok = ledger_bignum_alloc(n,digit_count,n->point_place);
        if (!ok) return 0;
      }
    }
    /* set the number value */{
      int pos;
      unsigned long int modal_nv = nv;
      /* clear the decimal portion */
      memset(n->digits,0,n->point_place*sizeof(unsigned char));
      /* place the rest of the digits */
      for (pos = n->point_place; pos < n->digit_count; ++pos){
        unsigned char next_digit = (unsigned char)(modal_nv%100u);
        n->digits[pos] = next_digit;
        modal_nv /= 100u;
      }
    }
    /* set the sign */
    n->negative = neg?1:0;
    /* done */
    return 1;
  }
}

long int ledger_bignum_get_long(struct ledger_bignum const* n){
  long int out;
  /* ignore the fractional part */{
    unsigned long int modal_out = 0;
    int pos;
    for (pos = n->digit_count-1; pos >= n->point_place; --pos){
      unsigned char const next_digit = n->digits[pos];
      if (modal_out >= ULONG_MAX/100){
        modal_out = ULONG_MAX;
        break;
      }
      modal_out *= 100u;
      if (modal_out >= ULONG_MAX-next_digit){
        modal_out = ULONG_MAX;
        break;
      }
      modal_out += next_digit;
    }
    if (n->negative){
      if (modal_out > (unsigned long int)LONG_MAX)
        out = LONG_MIN;
      else
        out = -(long int)modal_out;
    } else {
      if (modal_out >= (unsigned long int)+LONG_MAX)
        out = LONG_MAX;
      else
        out = +(long int)modal_out;
    }
  }
  return out;
}

int ledger_bignum_alloc
  (struct ledger_bignum* n, int digits, int point_place)
{
  /* sanitize the input */{
    if (digits > LEDGER_BIGNUM_DIGIT_MAX)
      return 0;
    if (digits < 0 || point_place < 0)
      return 0;
    else if (point_place > digits)
      return 0;
  }
  if (digits > 0){
    /* allocate new digit space */
    unsigned char* new_digits = (unsigned char*)ledger_util_malloc
      (sizeof(unsigned char)*(digits));
    if (new_digits == NULL){
      return 0;
    } else {
      ledger_util_free(n->digits);
      memset(new_digits,0,digits*sizeof(unsigned char));
      n->digits = new_digits;
      n->digit_count = digits;
      n->point_place = point_place;
      return 1;
    }
  } else {
    /* free up the digits */
    ledger_util_free(n->digits);
    n->digits = NULL;
    n->digit_count = 0;
    n->point_place = 0;
    return 1;
  }
}

int ledger_bignum_count_digits(struct ledger_bignum const* n){
  return n->digit_count;
}

int ledger_bignum_find_point(struct ledger_bignum const* n){
  return n->point_place;
}

/* END   implementation */
