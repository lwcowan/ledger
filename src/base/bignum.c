
#include "bignum.h"
#include "util.h"

/*
 * Actualization of the big number structure
 */
struct ledger_bignum {
  /* base-10 digits, stored in little-endian order */
  unsigned char *digits;
  /* number of digits, equal to twice the number of bytes */
  int digit_count;
  /* position of the decimal point, in digits */
  int point_place;
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
  return 1;
}

void ledger_bignum_clear(struct ledger_bignum* n){
  ledger_util_free(n->digits);
  n->digits = NULL;
  n->digit_count = 0;
  n->point_place = 0;
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

/* END   implementation */
