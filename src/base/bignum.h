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


#endif /*__Ledger_base_bigNum_H__*/
