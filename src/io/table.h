/*
 * file: io/table.h
 * brief: Table transport API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_table_H__
#define __Ledger_IO_table_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_table;

/*
 * Parse a CSV text. The text might be clobbered after parsing.
 * - table table to read into
 * - csv csv text
 * @return one on success, zero otherwise
 */
int ledger_io_table_parse_csv(struct ledger_table* table, unsigned char *csv);

/*
 * Print a CSV text.
 * - table table to write out
 * @return the text allocated on success, NULL otherwise
 */
unsigned char* ledger_io_table_print_csv(struct ledger_table const* table);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_table_H__*/
