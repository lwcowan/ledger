/*
 * file: io/book.h
 * brief: I/O API for account and transaction book
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_IO_book_H__
#define __Ledger_IO_book_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_book;

/*
 * Read a book file.
 * - filename name of book file to read
 * - book the book to receive the copy of the contents
 * @return one on success, zero otherwise
 */
int ledger_io_book_read(char const* filename, struct ledger_book* book);

/*
 * Write a book file.
 * - filename name of book file to write
 * - book the book to record into the file
 * @return one on success, zero otherwise
 */
int ledger_io_book_write
  (char const* filename, struct ledger_book const* book);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_IO_book_H__*/
