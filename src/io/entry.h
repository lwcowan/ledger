/*
 * file: io/table.h
 * brief: Table transport API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_entry_H__
#define __Ledger_IO_entry_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_entry;
struct cJSON;

/*
 * Parse a JSON structure.
 * - entry entry to read into
 * - json JSON structure
 * @return one on success, zero otherwise
 */
int ledger_io_entry_parse_json
  (struct ledger_entry* entry, struct cJSON const *json);

/*
 * Print a JSON text.
 * - entry entry to write out
 * @return the JSON allocated on success, NULL otherwise
 */
struct cJSON* ledger_io_entry_print_json(struct ledger_entry const* entry);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_entry_H__*/
