/*
 * file: io/util.h
 * brief: I/O Utility functions
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_util_H__
#define __Ledger_IO_util_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct zip_t;
struct ledger_bignum;
struct cJSON;

/*
 * Extract text from a zip archive.
 * - zip archive from which to extract
 * - name entry name
 * - ok success flag
 * @returns NULL if no entry by the given name was available (success),
 *   NULL on read fault (not success), or a pointer to the text string
 *   (success)
 */
unsigned char* ledger_io_util_extract_text
  (struct zip_t *zip, char const* name, int *ok);
/*
 * Extract JSON from a zip archive.
 * - zip archive from which to extract
 * - name entry name
 * - ok success flag
 * @returns NULL if no entry by the given name was available (success),
 *   NULL on read fault (not success), or a pointer to the JSON structure
 *   (success)
 */
struct cJSON* ledger_io_util_extract_json
  (struct zip_t *zip, char const* name, int *ok);
/*
 * Add JSON to a zip archive.
 * - zip archive to which to add
 * - name entry name
 * - text text string
 * @returns a success flag
 */
int ledger_io_util_archive_text
  (struct zip_t *zip, char const* name, unsigned char const* text);
/*
 * Add JSON to a zip archive.
 * - zip archive to which to add
 * - name entry name
 * - json JSON structure
 * - ok success flag
 * @returns a success flag
 */
int ledger_io_util_archive_json
  (struct zip_t *zip, char const* name, struct cJSON const* json);
/*
 * Construct a file name.
 * - buf buffer to receive the etnry name
 * - len length of file name
 * - tmp_num workspace for number rendering
 * - format %-style format
 * @return the number of chars written
 *
 * Supports:
 *   %i integer
 */
int ledger_io_util_construct_name
  (char* buf, int len, struct ledger_bignum* tmp_num, char const* format, ...);




#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_util_H__*/
