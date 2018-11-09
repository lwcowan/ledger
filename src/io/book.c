
#include "book.h"
#include "../../deps/zip/src/zip.h"

int ledger_io_book_read(char const* filename, struct ledger_book* book){
  struct zip_t *active_zip = zip_open(filename, 6, 'r');
  if (active_zip == NULL){
    return 0;
  } else {
    int result = 0;
    do {
      result = 1;
    } while (0);
    zip_close(active_zip);
    return result;
  }
}
int ledger_io_book_write
  (char const* filename, struct ledger_book const* book)
{
  struct zip_t *active_zip = zip_open(filename, 6, 'w');
  if (active_zip == NULL){
    return 0;
  } else {
    int result = 0;
    do {
      result = 1;
    } while (0);
    zip_close(active_zip);
    return result;
  }
}
