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

struct ledger_ledger;

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
/*
 * Query the sequence identifier of a book.
 * - b book to query
 * @return the next identifier
 */
int ledger_book_get_sequence(struct ledger_book const* b);
/*
 * Modify the sequence identifier of a book.
 * - b book to query
 * - item_id new identifier (non-negative)
 */
int ledger_book_set_sequence(struct ledger_book* b, int item_id);
/*
 * Allocate a sequence identifier from a book.
 * - b book to use
 * @return a new identifier (non-negative) on success,
 *   or -1 if out of identifiers
 */
int ledger_book_alloc_id(struct ledger_book* b);
/*
 * Get a ledger count.
 * - b book to query
 * @return the number of ledgers in this book
 */
int ledger_book_get_ledger_count(struct ledger_book const* b);
/*
 * Set a ledger count.
 * - b book to configure
 * - n number of ledgers
 * @return one on success, zero otherwise
 */
int ledger_book_set_ledger_count(struct ledger_book* b, int n);
/*
 * Get a ledger.
 * - b book to adjust
 * - i array index
 * @return the ledger at that array index
 */
struct ledger_ledger* ledger_book_get_ledger(struct ledger_book* b, int i);
/*
 * Get a ledger.
 * - b book to read
 * - i array index
 * @return the ledger at that array index
 */
struct ledger_ledger const* ledger_book_get_ledger_c
  (struct ledger_book const* b, int i);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_book_H__*/
