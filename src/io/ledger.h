/*
 * file: io/ledger.h
 * brief: I/O API for account ledger
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_ledger_H__
#define __Ledger_IO_ledger_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_ledger;
struct zip_t;
struct ledger_io_manifest;
struct ledger_bignum;

/*
 * Write zip entries for a single ledger.
 * - zip open zip archive for writing
 * - manifest transport manifest describing the ledger
 * - ledger the ledger to write
 * - tmp_num big number instance to use for composing number strings
 * @return one on success, zero otherwise
 */
int ledger_io_ledger_write_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_ledger const* ledger, struct ledger_bignum* tmp_num);

/*
 * Read zip entries for a single ledger.
 * - zip open zip archive for reading
 * - manifest transport manifest describing the ledger
 * - ledger the ledger to receive the read data
 * - tmp_num big number instance to use for composing number strings
 * @return one on success, zero otherwise
 */
int ledger_io_ledger_read_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_ledger* ledger, struct ledger_bignum* tmp_num);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_ledger_H__*/
