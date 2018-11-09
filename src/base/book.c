
#include "book.h"
#include "util.h"

/*
 * Actualization of the book structure
 */
struct ledger_book {
  /*
   * brief: book description
   */
  unsigned char *description;
  /*
   * brief: notes
   */
  unsigned char *notes;
};

/*
 * Initialize a book.
 * - book book to initialize
 * @return one on success, zero on failure
 */
static int ledger_book_init(struct ledger_book* book);
/*
 * Clear out a book.
 * - book book to clear
 */
static void ledger_book_clear(struct ledger_book* book);


/* BEGIN static implementation */

int ledger_book_init(struct ledger_book* book){
  book->description = NULL;
  book->notes = NULL;
  return 1;
}

void ledger_book_clear(struct ledger_book* book){
  ledger_util_free(book->description);
  book->description = NULL;
  ledger_util_free(book->notes);
  book->notes = NULL;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_book* ledger_book_new(void){
  struct ledger_book* book = (struct ledger_book* )ledger_util_malloc
    (sizeof(struct ledger_book));
  if (book != NULL){
    if (!ledger_book_init(book)){
      ledger_util_free(book);
      book = NULL;
    }
  }
  return book;
}

void ledger_book_free(struct ledger_book* book){
  if (book != NULL){
    ledger_book_clear(book);
    ledger_util_free(book);
  }
}

unsigned char const* ledger_book_get_description
  (struct ledger_book const* book)
{
  return book->description;
}

int ledger_book_set_description
  (struct ledger_book* book, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(book->description);
    book->description = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_book_get_notes(struct ledger_book const* book){
  return book->notes;
}

int ledger_book_set_notes
  (struct ledger_book* book, unsigned char const* notes)
{
  int ok;
  unsigned char* new_notes = ledger_util_ustrdup(notes,&ok);
  if (ok){
    ledger_util_free(book->notes);
    book->notes = new_notes;
    return 1;
  } else return 0;
}

int ledger_book_is_equal
  (struct ledger_book const* a, struct ledger_book const* b)
{
  /* trivial books */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (ledger_util_ustrcmp(a->description, b->description) != 0)
      return 0;
    if (ledger_util_ustrcmp(a->notes, b->notes) != 0)
      return 0;
  }
  return 1;
}


/* END   implementation */
