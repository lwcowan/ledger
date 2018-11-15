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
 * brief: Table row iterator
 */
struct ledger_table_mark;

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

/*
 * Get a mark pointing to the first row of a table.
 * - t table to modify
 * @return a mark at the start of the table
 */
struct ledger_table_mark* ledger_table_begin(struct ledger_table* t);

/*
 * Get a mark pointing to the first row of a table.
 * - t table to read
 * @return a mark at the start of the table
 */
struct ledger_table_mark* ledger_table_begin_c(struct ledger_table const* t);

/*
 * Get a mark pointing to after the last row of a table.
 * - t table to modify
 * @return a mark at the end of the table
 */
struct ledger_table_mark* ledger_table_end(struct ledger_table* t);

/*
 * Get a mark pointing to after the last row of a table.
 * - t table to read
 * @return a mark at the end of the table
 */
struct ledger_table_mark* ledger_table_end_c(struct ledger_table const* t);

/*
 * Compare two marks.
 * - a one mark
 * - b another mark
 * @return one if the marks point to the same row, zero otherwise
 */
int ledger_table_mark_is_equal
  (struct ledger_table_mark const* a, struct ledger_table_mark const* b);

/*
 * Free a mark.
 * - m mark to free
 * @return the mark on success, NULL otherwise
 */
void ledger_table_mark_free(struct ledger_table_mark* m);

/*
 * Query the number of table columns.
 * - t table to query
 * @return a column count
 */
int ledger_table_get_column_count(struct ledger_table const* t);

/*
 * Query the type code for a table column.
 * - t table to query
 * - c column index
 * @return a type code for the column, or zero if the
 *   column is unavailable
 */
int ledger_table_get_column_type(struct ledger_table const* t, int i);

/*
 * Configure a table's columns.
 * - t table to query
 * - n number of columns
 * - types array of types
 * @return one on success, zero otherwise
 */
int ledger_table_set_column_types
  (struct ledger_table* t, int n, int const* types);

/*
 * Query a table's row count.
 * - t table to query
 * @return a table's row count
 */
int ledger_table_count_rows(struct ledger_table const* t);

/*
 * Add a row just before the mark's current position.
 * The mark will then point to the new row.
 * - mark a mutable mark
 * @return one on success, zero otherwise
 */
int ledger_table_add_row(struct ledger_table_mark* mark);

/*
 * Drop the row at the mark's current position.
 * The mark will then point to the previous row.
 * - mark a mutable mark
 * @return one on success, zero otherwise
 */
int ledger_table_drop_row(struct ledger_table_mark* mark);

/*
 * Fetch a row item as a string.
 * - mark any mark
 * - i column index
 * - buf buffer to hold text string
 * - len size of the buffer
 * @return the number of bytes needed to hold the string on success,
 *   negative otherwise
 */
int ledger_table_fetch_string
  (struct ledger_table_mark const* mark, int i, unsigned char* buf, int len);

/*
 * Put a string value to a row item.
 * - mark any mark
 * - i column index
 * - value NUL-terminated string
 * @return one on success, zero otherwise
 */
int ledger_table_put_string
  (struct ledger_table_mark const* mark, int i, unsigned char const* value);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_table_H__*/
