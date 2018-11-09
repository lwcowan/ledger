/*
 * file: io/manifest.h
 * brief: I/O manifest API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_manifest_H__
#define __Ledger_IO_manifest_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_book;

/*
 * brief: Ancillary information flags
 */
enum ledger_io_manifest_flag {
  LEDGER_IO_MANIFEST_DESC = 1,
  LEDGER_IO_MANIFEST_NOTES = 2
};
/*
 * brief: Book manifest
 */
struct ledger_io_manifest;

/*
 * Construct a new manifest structure.
 * @return the manifest on success, otherwise NULL
 */
struct ledger_io_manifest* ledger_io_manifest_new(void);
/*
 * Destroy a manifest.
 * - m the manifest to destroy
 */
void ledger_io_manifest_free(struct ledger_io_manifest* m);
/*
 * Get top-level flags for a manifest.
 * - m manifest to query
 * @return the top-level flags
 */
int ledger_io_manifest_get_top_flags(struct ledger_io_manifest const* m);
/*
 * Set top-level flags for a manifest.
 * - m manifest to query
 * - flags new top flags
 */
void ledger_io_manifest_set_top_flags(struct ledger_io_manifest* m, int flags);
/*
 * Prepare a manifest of a book.
 * - manifest manifest to adjust
 * - book book to read
 * @return one on success, zero otherwise
 */
int ledger_io_manifest_prepare
  (struct ledger_io_manifest* manifest, struct ledger_book* book);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_manifest_H__*/
