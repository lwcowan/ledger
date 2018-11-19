
#include "iocmd.h"
#include "../base/book.h"
#include "../io/book.h"
#include "line.h"
#include <stdio.h>

int ledger_cli_read(struct ledger_cli_line *tracking, int argc, char **argv){
  int result = 1;
  struct ledger_book *new_book;
  if (argc < 2){
    fputs("read: Read a book from a file.\n"
      "usage: read (filename)\n",stderr);
    return 2;
  }
  new_book = ledger_book_new();
  if (new_book == NULL){
    fputs("Error encountered when allocating new book.\n",stderr);
    return 1;
  } else do {
    if (!ledger_io_book_read(argv[1], new_book)){
      break;
    }
    result = 0;
  } while (0);
  if (result != 0){
    ledger_book_free(new_book);
    fputs("Loading of book encountered errors.\n",stderr);
  } else {
    ledger_book_free(tracking->book);
    tracking->book = new_book;
    fputs("Load done.\n",stderr);
  }
  return 0;
}


int ledger_cli_write(struct ledger_cli_line *tracking, int argc, char **argv){
  int result = 1;
  if (argc < 2){
    fputs("write: Write a book to a file.\n"
      "usage: write (filename)\n",stderr);
    return 2;
  }
  do {
    if (!ledger_io_book_write(argv[1], tracking->book)){
      break;
    }
    result = 0;
  } while (0);
  if (result != 0){
    fputs("Saving of book encountered errors.\n",stderr);
  } else {
    fputs("Save done.\n",stderr);
  }
  return 0;
}
