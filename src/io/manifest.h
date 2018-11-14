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
struct cJSON;
struct ledger_ledger;
struct ledger_account;

/*
 * brief: Ancillary information flags
 */
enum ledger_io_manifest_flag {
  LEDGER_IO_MANIFEST_DESC = 1,
  LEDGER_IO_MANIFEST_NOTES = 2,
  LEDGER_IO_MANIFEST_NAME = 4
};
/*
 * brief: Manifest type codes
 */
enum ledger_io_manifest_type {
  LEDGER_IO_MANIFEST_BOOK = 1,
  LEDGER_IO_MANIFEST_LEDGER = 2,
  LEDGER_IO_MANIFEST_ACCOUNT = 3
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
 * Get a manifest's type code.
 * - m manifest to query
 * @return its type code
 */
int ledger_io_manifest_get_type(struct ledger_io_manifest const* m);
/*
 * Set the type code for a manifest.
 * - m manifest to query
 * - typ a new type code
 */
void ledger_io_manifest_set_type(struct ledger_io_manifest* m, int typ);
/*
 * Get a manifest's identification number.
 * - m manifest to query
 * @return its identification number
 */
int ledger_io_manifest_get_id(struct ledger_io_manifest const* m);
/*
 * Set the identification number for a manifest.
 * - m manifest to query
 * - item_id a new identification number
 */
void ledger_io_manifest_set_id(struct ledger_io_manifest* m, int item_id);
/*
 * Prepare a manifest of a book.
 * - manifest manifest to adjust
 * - book book to read
 * @return one on success, zero otherwise
 */
int ledger_io_manifest_prepare
  (struct ledger_io_manifest* manifest, struct ledger_book const* book);
/*
 * Prepare a manifest of a ledger.
 * - manifest manifest to adjust
 * - ledger ledger to read
 * @return one on success, zero otherwise
 */
int ledger_io_manifest_prepare_ledger
  (struct ledger_io_manifest* manifest, struct ledger_ledger const* ledger);
/*
 * Prepare a manifest of an account.
 * - manifest manifest to adjust
 * - account account to read
 * @return one on success, zero otherwise
 */
int ledger_io_manifest_prepare_account
  (struct ledger_io_manifest* manifest, struct ledger_account const* account);
/*
 * Convert a manifest to a JSON object.
 * - manifest the manifest to convert
 * @return the JSON object on success
 */
struct cJSON* ledger_io_manifest_print
  (struct ledger_io_manifest const* manifest);
/*
 * Convert a manifest from a JSON object.
 * - manifest the manifest to update
 * - json JSON object to update
 * - typ type code for new manifest contents
 * @return nonzero on success
 */
int ledger_io_manifest_parse
  (struct ledger_io_manifest* manifest, struct cJSON const* json, int typ);

/*
 * Get a sub-manifest count.
 * - b manifest to query
 * @return the number of sub-manifests in this manifest
 */
int ledger_io_manifest_get_count(struct ledger_io_manifest const* b);
/*
 * Set a sub-manifest count.
 * - b manifest to configure
 * - n number of sub-manifests
 * @return one on success, zero otherwise
 */
int ledger_io_manifest_set_count(struct ledger_io_manifest* b, int n);
/*
 * Get a sub-manifest.
 * - b manifest to adjust
 * - i array index
 * @return the sub-manifest at that array index
 */
struct ledger_io_manifest* ledger_io_manifest_get
  (struct ledger_io_manifest* b, int i);
/*
 * Get a sub-manifest.
 * - b manifest to read
 * - i array index
 * @return the sub-manifest at that array index
 */
struct ledger_io_manifest const* ledger_io_manifest_get_c
  (struct ledger_io_manifest const* b, int i);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_manifest_H__*/
