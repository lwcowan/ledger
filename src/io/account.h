/*
 * file: io/account.h
 * brief: I/O API for accounts
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_account_H__
#define __Ledger_IO_account_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_account;
struct zip_t;
struct ledger_io_manifest;
struct ledger_bignum;

/*
 * Write zip entries for a single account.
 * - zip open zip archive for writing
 * - manifest transport manifest describing the account
 * - account the account to write
 * - tmp_num big number instance to use for composing number strings
 * - ledger_id identifier of the containing ledger
 * @return one on success, zero otherwise
 */
int ledger_io_account_write_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_account const* ledger, struct ledger_bignum* tmp_num,
    int ledger_id);

/*
 * Read zip entries for a single account.
 * - zip open zip archive for reading
 * - manifest transport manifest describing the account
 * - account the account to receive the read data
 * - tmp_num big number instance to use for composing number strings
 * - ledger_id identifier of the containing ledger
 * @return one on success, zero otherwise
 */
int ledger_io_account_read_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_account* ledger, struct ledger_bignum* tmp_num,
    int ledger_id);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_account_H__*/
