
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

/*
 * Zero out a number.
 * - n number to zero
 */
static void ledger_bignum_zero_all(struct ledger_bignum* n);

/*
 * Zero out the fractional part of a number.
 * - n number to zero
 */
static void ledger_bignum_zero_cents(struct ledger_bignum* n);

/*
 * Allocate and partition only more space for a big number.
 * This function resets the number to zero.
 * - n the number structure to configure
 * - digits number of base-100 digits to allocate
 * - point_place position of centesimal point; must be less
 *     than or equal to `digits`
 * @return one on success, zero otherwise
 */
static int ledger_bignum_extend
  (struct ledger_bignum* n, int digits, int point_place);

/*
 * Allocate and partition space for a big number without
 * checking arguments. This function resets the number to zero.
 * - n the number structure to configure
 * - digits number of base-100 digits to allocate
 * - point_place position of centesimal point; must be less
 *     than or equal to `digits`
 * @return one on success, zero otherwise
 */
static int ledger_bignum_alloc_unchecked
  (struct ledger_bignum* n, int digits, int point_place);

/*
 * Fetch a centesimal digit from a big number.
 * - n number to read
 * - i digit place
 * @return the digit, or zero if `i` is out of range
 */
static int ledger_bignum_fetch_zero(struct ledger_bignum const* n, int i);


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

void ledger_bignum_zero_all(struct ledger_bignum* n){
  memset(n->digits,0,n->digit_count*sizeof(unsigned char));
}

void ledger_bignum_zero_cents(struct ledger_bignum* n){
  memset(n->digits,0,n->point_place*sizeof(unsigned char));
}

int ledger_bignum_extend
  (struct ledger_bignum* n, int digits, int point_place)
{
  /* sanitize the input */{
    if (digits > LEDGER_BIGNUM_DIGIT_MAX)
      return 0;
    else if (digits < 0 || point_place < 0)
      return 0;
    else if (point_place > digits)
      return 0;
  }
  /* skip allocation */if (point_place <= n->point_place
    &&  digits <= n->digit_count)
  {
    ledger_bignum_zero_all(n);
    return 1;
  }
  /* extend the arguments */
  if (point_place < n->point_place) point_place = n->point_place;
  if (digits-point_place < n->digit_count-n->point_place){
    digits = n->digit_count-n->point_place+point_place;
  }
  return ledger_bignum_alloc_unchecked(n,digits,point_place);
}

int ledger_bignum_alloc_unchecked
  (struct ledger_bignum* n, int digits, int point_place)
{
  if (digits > 0){
    /* allocate new digit space */
    unsigned char* new_digits = (unsigned char*)ledger_util_malloc
      (sizeof(unsigned char)*(digits));
    if (new_digits == NULL){
      return 0;
    } else {
      ledger_util_free(n->digits);
      n->digits = new_digits;
      n->digit_count = digits;
      n->point_place = point_place;
      ledger_bignum_zero_all(n);
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

int ledger_bignum_fetch_zero(struct ledger_bignum const* n, int i){
  if (i < 0 || i >= n->digit_count) return 0;
  else return n->digits[i];
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
  int a_start, a_end;
  int b_start, b_end;
  int sign;
  /* ensure that signed zeroes compare equal */{
    if (ledger_util_uiszero(a->digits,a->digit_count)
    &&  ledger_util_uiszero(b->digits,b->digit_count))
      return 0;
  }
  /* optimize for sign differences */
  if (a->negative && (!b->negative)){
    return -1;
  } else if (a->negative && (!b->negative)){
    return +1;
  } else {
    /* same sign */
    sign = a->negative?-1:+1;
  }

  /* align starts */{
    if (a->point_place < b->point_place){
      /* `a` has less digits */
      a_start = (a->point_place - b->point_place);
      b_start = 0;
    } else {
      /* `b` has less digits */
      a_start = 0;
      b_start = (b->point_place - a->point_place);
    }
  }
  /* align ends */{
    int const a_integral = a->digit_count - a->point_place;
    int const b_integral = b->digit_count - b->point_place;
    if (a_integral < b_integral){
      /* `a` has less integer digits */
      a_end = a->digit_count + (b_integral - a_integral);
      b_end = b->digit_count;
    } else {
      /* `b` has less integer digits */
      a_end = a->digit_count;
      b_end = b->digit_count + (a_integral - b_integral);
    }
  }
  /* compare from end */{
    int a_i, b_i;
    for (a_i = a_end-1, b_i = b_end-1; a_i >= a_start && b_i >= b_start;
          --a_i, --b_i)
    {
      int const a_char = ledger_bignum_fetch_zero(a,a_i);
      int const b_char = ledger_bignum_fetch_zero(b,b_i);
      if (a_char < b_char) return -1*sign;
      else if (a_char > b_char) return +1*sign;
    }
  }
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
      int const ok = ledger_bignum_extend
        (n,digit_count+n->point_place,n->point_place);
      if (!ok) return 0;
    }
    /* set the number value */{
      int pos;
      unsigned long int modal_nv = nv;
      /* clear the decimal portion */
      ledger_bignum_zero_cents(n);
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
  return ledger_bignum_alloc_unchecked(n,digits,point_place);
}

int ledger_bignum_count_digits(struct ledger_bignum const* n){
  return n->digit_count;
}

int ledger_bignum_find_point(struct ledger_bignum const* n){
  return n->point_place;
}

int ledger_bignum_set_text
  ( struct ledger_bignum* n, unsigned char const* text,
    unsigned char** endptr)
{
  size_t str_extent, fraction_extent;
  size_t str_count, fraction_count;
  unsigned short complement_extent = 0;
  unsigned short dot_extent = 0;
  /* compute the length of the valid number */{
    str_extent = 0;
    /* recognize the minus sign */
    if (text[str_extent] == '-' || text[str_extent] == '+'){
      str_extent = 1;
      complement_extent = 1;
    }
    /* recognize numeric digits */
    while (text[str_extent] >= '0' && text[str_extent] <= '9'){
      str_extent += 1;
    }
    if (str_extent == complement_extent){
      /* no number here */
      str_extent = 0;
    } else if (text[str_extent] == '.'){
      /* recognize decimal portion */
      str_extent += 1;
      fraction_extent = 0;
      dot_extent = 1;
      while (text[str_extent] >= '0' && text[str_extent] <= '9'){
        str_extent += 1;
        fraction_extent += 1;
      }
    } else fraction_extent = 0;
  }
  /* set end pointer */if (endptr != NULL){
    *endptr = (unsigned char*)(text+str_extent);
  }
  /* adjust to even, while watching string length */{
    str_count = str_extent-dot_extent-complement_extent;
    fraction_count = fraction_extent;
    if (str_count >= INT_MAX)
      return 0;
    if (fraction_count % 2 != 0){
      fraction_count += 1;
      str_count += 1;
    }
    if (str_count >= INT_MAX)
      return 0;
    if (str_count % 2 != 0){
      str_count += 1;
    }
    if (str_count >= INT_MAX)
      return 0;
  }
  if (str_extent > 0){
    /* prepare space for the number */
    int const digit_count = (int)(str_count/2);
    int const point_place = (int)(fraction_count/2);
    int const ok = ledger_bignum_extend(n,digit_count,point_place);
    int real_execution_start;
    if (!ok) return 0;
    real_execution_start = n->point_place+(digit_count-point_place);
    /* transfer number */{
      int i, read_point = 0;
      if (complement_extent){
        switch (text[read_point]){
          case '-':
            n->negative = 1;
            break;
          case '+':
            n->negative = 0;
            break;
        }
        read_point += 1;
      } else n->negative = 0;
      /* put the integral portion */{
        if (real_execution_start == n->point_place){
          /* fetch a zero */;
          i = real_execution_start;
        } else {
          /* first digit */{
            unsigned char next_digit;
            size_t const integral_length =
              (str_extent-fraction_extent-dot_extent-complement_extent);
            i = real_execution_start-1;
            if (integral_length%2 == 0){
              /* get two digits */
              if (read_point < str_extent)
                next_digit = (text[read_point++]-'0')*10;
              if (read_point < str_extent)
                next_digit += (text[read_point++]-'0');
            } else {
              /* get one digit */
              if (read_point < str_extent)
                next_digit = (text[read_point++]-'0');
            }
            n->digits[i] = next_digit;
          }
          for (; i > n->point_place; --i){
            /* get two digits */
            unsigned char next_digit = 0;
            if (read_point < str_extent)
              next_digit = (text[read_point++]-'0')*10;
            if (read_point < str_extent)
              next_digit += (text[read_point++]-'0');
            n->digits[i-1] = next_digit;
          }
        }
      }
      /* put the fractional portion */if (text[read_point] == '.'){
        read_point += 1;
        i -= 1;
        for (; i >= 0; --i){
          /* get two digits */
          unsigned char next_digit = 0;
          if (read_point < str_extent)
            next_digit = (text[read_point++]-'0')*10;
          if (read_point < str_extent)
            next_digit += (text[read_point++]-'0');
          n->digits[i] = next_digit;
        }
      }
    }/* end transfer number */
  } else {
    ledger_bignum_zero_all(n);
  }
  return 1;
}

int ledger_bignum_get_text
  (struct ledger_bignum const* n, unsigned char* buf, int len, int want_plus)
{
  int byte_count;
  int real_execution_start;
  /* compute number of bytes needed to represent the number */{
    if (n->digit_count > 0){
      int i;
      byte_count = n->digit_count*2;
      for (i = n->digit_count; i > n->point_place; --i){
        if (n->digits[i-1] > 0)
          break;
        else byte_count -= 2;
      }
      if (i > n->point_place){
        if (n->digits[i-1] < 10){
          /* drop front zero */
          byte_count -= 1;
        }
      } else {
        /* add front zero */
        byte_count += 1;
      }
      real_execution_start = i;
    } else {
      byte_count = 1;
    }
    if (n->negative || want_plus){
      /* add minus sign */
      byte_count += 1;
    }
    if (n->point_place > 0){
      /* add decimal point */
      byte_count += 1;
    }
  }
  /* construct the string */if (buf != NULL && len > 0){
    int i, write_point = 0;
    if (n->negative){
      if (write_point < len) buf[write_point++] = '-';
    } else if (want_plus){
      if (write_point < len) buf[write_point++] = '+';
    }
    /* put the integral portion */if (n->digit_count > 0){
      if (real_execution_start == n->point_place){
        /* plop a zero down */
        if (write_point < len) buf[write_point++] = '0';
        i = real_execution_start;
      } else {
        /* first digit */
        if (n->digits[real_execution_start-1] >= 10){
          /* put two digits */
          if (write_point < len)
            buf[write_point++] = '0'+(n->digits[real_execution_start-1]/10);
          if (write_point < len)
            buf[write_point++] = '0'+(n->digits[real_execution_start-1]%10);
        } else {
          /* put one digit */
          if (write_point < len)
            buf[write_point++] = '0'+(n->digits[real_execution_start-1]);
        }
        i = real_execution_start-1;
        for (; i > n->point_place; --i){
          /* put two digits */
          if (write_point < len)
            buf[write_point++] = '0'+(n->digits[i-1]/10);
          if (write_point < len)
            buf[write_point++] = '0'+(n->digits[i-1]%10);
        }
      }
    } else {
      if (write_point < len)
        buf[write_point++] = '0';
    }
    /* put the fractional portion */if (n->point_place > 0){
      if (write_point < len) buf[write_point++] = '.';
      i -= 1;
      for (; i >= 0; --i){
        /* put two digits */
        if (write_point < len)
          buf[write_point++] = '0'+(n->digits[i]/10);
        if (write_point < len)
          buf[write_point++] = '0'+(n->digits[i]%10);
      }
    }
    /* NUL-terminate the string */{
      if (write_point < len) buf[write_point] = 0;
      else buf[len-1] = 0;
    }
  }
  return byte_count;
}

void ledger_bignum_swap(struct ledger_bignum* n, struct ledger_bignum* n2){
  unsigned char *tmp_p;
  int tmp_i;
  tmp_p = n->digits; n->digits = n2->digits; n2->digits = tmp_p;
  tmp_i = n->digit_count;n->digit_count=n2->digit_count;n2->digit_count=tmp_i;
  tmp_i = n->point_place;n->point_place=n2->point_place;n2->point_place=tmp_i;
  tmp_i = n->negative;n->negative=n2->negative;n2->negative=tmp_i;
  return;
}

int ledger_bignum_copy
  (struct ledger_bignum* dst, struct ledger_bignum const* src)
{
  int const digit_count = src->digit_count;
  int ok =
    ledger_bignum_alloc_unchecked(dst, digit_count, src->point_place);
  if (!ok) return 0;
  memcpy(dst->digits, src->digits, digit_count);
  dst->negative = src->negative;
  return 1;
}

int ledger_bignum_assign
  (struct ledger_bignum* dst, struct ledger_bignum const* src)
{
  int const digit_count = src->digit_count;
  int ok =
    ledger_bignum_extend(dst, digit_count, src->point_place);
  int const disparity = dst->point_place-src->point_place;
  if (!ok) return 0;
  memcpy(dst->digits+disparity, src->digits, digit_count);
  dst->negative = src->negative;
  return 1;
}

int ledger_bignum_truncate
  (struct ledger_bignum* dst, struct ledger_bignum const* src)
{
  int digit_count = src->digit_count;
  int initial_digit = 0;
  int disparity = dst->point_place-src->point_place;
  if (disparity < 0){
    initial_digit = -disparity;
    digit_count += disparity;
    disparity = 0;
  }
  if (digit_count+disparity > dst->digit_count){
    digit_count = dst->digit_count-disparity;
  }
  if (digit_count > 0)
    memcpy(dst->digits+disparity, src->digits+initial_digit, digit_count);
  dst->negative = src->negative;
  return 1;
}

/* END   implementation */
