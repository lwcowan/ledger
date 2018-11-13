/*
 * file: base/bignum.h
 * brief: Big number functions
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_bigNum_H__
#define __Ledger_base_bigNum_H__

/*
 * Big number, with movable decimal point
 */
struct ledger_bignum;


/*
 * Construct a new big number, equal to zero.
 * @return the number on success, otherwise NULL
 */
struct ledger_bignum* ledger_bignum_new(void);
/*
 * Destroy a big number.
 * - n the number to destroy
 */
void ledger_bignum_free(struct ledger_bignum* n);
/*
 * Compare two big numbers.
 * - a one big number
 * - b another big number
 * @return zero if equal, negative if `a` is less than `b`,
 *   positive if `a` is greater than `b`
 */
int ledger_bignum_compare
  (struct ledger_bignum const* a, struct ledger_bignum const* b);
/*
 * Assign a long integer value to a big number.
 * - n the number to modify
 * - v long integer value
 * @return one on success, zero otherwise
 */
int ledger_bignum_set_long(struct ledger_bignum* n, long int v);
/*
 * Retrieve a long integer from a big number.
 * - n the number to read
 * @return a long integer value, or LONG_MAX or LONG_MIN on overflow
 */
long int ledger_bignum_get_long(struct ledger_bignum const* n);


#endif /*__Ledger_base_bigNum_H__*/
