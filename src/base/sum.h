/*
 * file: base/sum.h
 * brief: Sum columns of big numbers
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_Sum_H__
#define __Ledger_base_Sum_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_bignum;
struct ledger_table;

/*
 * Sum a column of big numbers.
 * - out output number, to hold the sum
 * - table table over which to sum
 * - column column selector
 * @return one on success, zero otherwise
 */
int ledger_sum_table_column
  (struct ledger_bignum* out, struct ledger_table const* table, int column);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_Sum_H__*/
