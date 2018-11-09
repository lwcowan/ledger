/*
 * file: base/book.h
 * brief: Account and transaction book API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_book_H__
#define __Ledger_base_book_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * brief: Account and transaction book
 */
struct ledger_book;


/*
 * Construct a new book.
 * @return the book on success, otherwise NULL
 */
struct ledger_book* ledger_book_new(void);
/*
 * Destroy a book.
 * - book the book to destroy
 */
void ledger_book_free(struct ledger_book* book);
/*
 * Query the description of a book.
 * - book book to query
 * @return the description if available, otherwise NULL
 */
unsigned char const* ledger_book_get_description
  (struct ledger_book const* book);
/*
 * Modify the description of a book.
 * - book book to query
 * - desc new description
 * @return one on success, zero otherwise
 */
int ledger_book_set_description
  (struct ledger_book* book, unsigned char const* desc);
/*
 * Query the notes in a book.
 * - book book to query
 * @return the notes if available, otherwise NULL
 */
unsigned char const* ledger_book_get_notes
  (struct ledger_book const* book);
/*
 * Modify the notes of a book.
 * - book book to query
 * - desc new notes
 * @return one on success, zero otherwise
 */
int ledger_book_set_notes
  (struct ledger_book* book, unsigned char const* notes);
/*
 * Compare two books for equality.
 * - a a book
 * - b another book
 * @return one if the books are equal, zero otherwise
 */
int ledger_book_is_equal
  (struct ledger_book const* a, struct ledger_book const* b);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_book_H__*/
