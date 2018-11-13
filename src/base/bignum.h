/*
 * file: base/bignum.h
 * brief: Big number functions
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_bigNum_H__
#define __Ledger_base_bigNum_H__

/*
 * Big number, with movable centesimal point
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

/*
 * Allocate and partition space for a big number.
 * This function resets the number to zero.
 * - n the number structure to configure
 * - digits number of base-100 digits to allocate
 * - point_place position of centesimal point; must be less
 *     than or equal to `digits`
 * @return one on success, zero otherwise
 */
int ledger_bignum_alloc
  (struct ledger_bignum* n, int digits, int point_place);

/*
 * Query the total number of base-100 digits used by this number.
 * - n the number to query
 * @return a count of integer and fractional digits
 */
int ledger_bignum_count_digits(struct ledger_bignum const* n);

/*
 * Query the position of the centesimal point.
 * - n the number to query
 * @return the base-100 position of the centesimal point
 */
int ledger_bignum_find_point(struct ledger_bignum const* n);

/*
 * Assign a long integer value to a big number.
 * - n the number to modify
 * - text string holding a numerical value (format: NNN.nnn)
 * - endptr if available, the end-of-number is written here
 * @return one on success, zero otherwise
 */
int ledger_bignum_set_text
  ( struct ledger_bignum* n, unsigned char const* text,
    unsigned char** endptr);

/*
 * Retrieve a long integer from a big number.
 * - n the number to read
 * - buf buffer to receive the NUL-terminated string
 * - len length of buffer in bytes
 * - want_plus whether to force a plus sign to output
 * @return the buffer size needed to hold the number
 */
int ledger_bignum_get_text
  (struct ledger_bignum const* n, unsigned char* buf, int len, int want_plus);

/*
 * Swap two big numbers.
 * - n a number
 * - n2 another number
 */
void ledger_bignum_swap(struct ledger_bignum* n, struct ledger_bignum* n2);

#endif /*__Ledger_base_bigNum_H__*/
