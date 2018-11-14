/*
 * file: base/table.h
 * brief: Value table structure
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_table_H__
#define __Ledger_base_table_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * brief: Table column types
 */
enum ledger_table_type {
  /* identifier */
  LEDGER_TABLE_ID = 1,
  /* big number */
  LEDGER_TABLE_BIGNUM = 2,
  /* unsigned character string */
  LEDGER_TABLE_USTR = 3
};
/*
 * brief: Table
 */
struct ledger_table;

/*
 * Construct a new table.
 * @return the account on success, otherwise NULL
 */
struct ledger_table* ledger_table_new(void);

/*
 * Destroy a table.
 * - a the table to destroy
 */
void ledger_table_free(struct ledger_table* t);

/*
 * Compare two tables for equality.
 * - a a table
 * - b another table
 * @return one if the tables are equal, zero otherwise
 */
int ledger_table_is_equal
  (struct ledger_table const* a, struct ledger_table const* b);



#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_table_H__*/
