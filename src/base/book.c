
#include "book.h"
#include "util.h"
#include "ledger.h"
#include <limits.h>

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
  /*
   * brief: ledger count
   */
  int ledger_count;
  /*
   * brief: array of ledgers
   */
  struct ledger_ledger** ledgers;
  /*
   * next id to use
   */
  int sequence_id;
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
  book->sequence_id = 0;
  book->ledgers = NULL;
  book->ledger_count = 0;
  return 1;
}

void ledger_book_clear(struct ledger_book* book){
  ledger_book_set_ledger_count(book,0);
  ledger_util_free(book->description);
  book->description = NULL;
  ledger_util_free(book->notes);
  book->notes = NULL;
  book->sequence_id = 0;
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
  /* compare ledgers */{
    int i;
    if (a->ledger_count != b->ledger_count) return 0;
    else for (i = 0; i < a->ledger_count; ++i){
      if (!ledger_ledger_is_equal(a->ledgers[i], b->ledgers[i]))
        break;
    }
    if (i < a->ledger_count) return 0;
  }
  return 1;
}

int ledger_book_get_sequence(struct ledger_book const* b){
  return b->sequence_id;
}

int ledger_book_set_sequence(struct ledger_book* b, int item_id){
  if (item_id < 0) return 0;
  b->sequence_id = item_id;
  return 1;
}

int ledger_book_alloc_id(struct ledger_book* b){
  if (b->sequence_id < INT_MAX){
    int out;
    out = b->sequence_id;
    b->sequence_id += 1;
    return out;
  } else return -1;
}

int ledger_book_get_ledger_count(struct ledger_book const* b){
  return b->ledger_count;
}
struct ledger_ledger* ledger_book_get_ledger(struct ledger_book* b, int i){
  if (i < 0 || i >= b->ledger_count){
    return NULL;
  } else {
    return b->ledgers[i];
  }
}
struct ledger_ledger const* ledger_book_get_ledger_c
  (struct ledger_book const* b, int i)
{
  if (i < 0 || i >= b->ledger_count){
    return NULL;
  } else {
    return b->ledgers[i];
  }
}
int ledger_book_set_ledger_count(struct ledger_book* b, int n){
  if (n >= INT_MAX/sizeof(struct ledger_ledger*)){
    return 0;
  } else if (n < 0){
    return 0;
  } else if (n == 0){
    int i;
    for (i = 0; i < b->ledger_count; ++i){
      ledger_ledger_free(b->ledgers[i]);
    }
    ledger_util_free(b->ledgers);
    b->ledgers = NULL;
    b->ledger_count = 0;
    return 1;
  } else if (n < b->ledger_count){
    int i;
    /* allocate smaller array */
    struct ledger_ledger** new_array = (struct ledger_ledger** )
      ledger_util_malloc(n*sizeof(struct ledger_ledger*));
    if (new_array == NULL) return 0;
    /* move old ledgers to new array */
    for (i = 0; i < n; ++i){
      new_array[i] = b->ledgers[i];
    }
    /* free rest of the ledgers */
    for (; i < b->ledger_count; ++i){
      ledger_ledger_free(b->ledgers[i]);
    }
    ledger_util_free(b->ledgers);
    b->ledgers = new_array;
    b->ledger_count = n;
    return 1;
  } else if (n >= b->ledger_count){
    int save_id;
    int i;
    /* allocate larger array */
    struct ledger_ledger** new_array = (struct ledger_ledger** )
      ledger_util_malloc(n*sizeof(struct ledger_ledger*));
    if (new_array == NULL) return 0;
    /* save the sequence number in case of rollback */
    save_id = b->sequence_id;
    /* make new ledgers */
    for (i = b->ledger_count; i < n; ++i){
      int next_id = ledger_book_alloc_id(b);
      if (next_id == -1) break;
      new_array[i] = ledger_ledger_new();
      if (new_array[i] == NULL) break;
      ledger_ledger_set_id(new_array[i], next_id);
    }
    /* rollback and quit */if (i < n){
      int j;
      /* rollback */
      for (j = b->ledger_count; j < i; ++j){
        ledger_ledger_free(new_array[i]);
      }
      b->sequence_id = save_id;
      /* quit */
      return 0;
    }
    /* transfer old ledgers */
    for (i = 0; i < b->ledger_count; ++i){
      new_array[i] = b->ledgers[i];
    }
    /* continue */
    ledger_util_free(b->ledgers);
    b->ledgers = new_array;
    b->ledger_count = n;
    return 1;
  } else return 1 /*since n == b->ledger_count */;
}

/* END   implementation */
