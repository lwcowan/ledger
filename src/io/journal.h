/*
 * file: io/journal.h
 * brief: I/O API for account journal
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_journal_H__
#define __Ledger_IO_journal_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_journal;
struct zip_t;
struct ledger_io_manifest;
struct ledger_bignum;

/*
 * Write zip entries for a single journal.
 * - zip open zip archive for writing
 * - manifest transport manifest describing the journal
 * - journal the journal to write
 * - tmp_num big number instance to use for composing number strings
 * @return one on success, zero otherwise
 */
int ledger_io_journal_write_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_journal const* journal, struct ledger_bignum* tmp_num);

/*
 * Read zip entries for a single journal.
 * - zip open zip archive for reading
 * - manifest transport manifest describing the journal
 * - journal the journal to receive the read data
 * - tmp_num big number instance to use for composing number strings
 * @return one on success, zero otherwise
 */
int ledger_io_journal_read_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_journal* journal, struct ledger_bignum* tmp_num);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_journal_H__*/
