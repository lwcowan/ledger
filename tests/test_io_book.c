
#include "../src/base/book.h"
#include "../src/io/book.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int io_write_zero_test(char const* );
static int io_write_nonzero_test(char const* );

struct test_struct {
  int (*fn)(char const* );
  char const* name;
};
struct test_struct test_array[] = {
  { io_write_zero_test, "i/o write zero" },
  { io_write_nonzero_test, "i/o write nonzero" }
};

int io_write_zero_test(char const* fn){
  int result = 0;
  struct ledger_book* ptr, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  ptr = ledger_book_new();
  if (ptr == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    ok = ledger_io_book_write(fn,ptr);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,ptr)) break;
    result = 1;
  } while (0);
  ledger_book_free(ptr);
  ledger_book_free(back_book);
  return result;
}
int io_write_nonzero_test(char const* fn){
  int result = 0;
  struct ledger_book* book, * back_book;
  back_book = ledger_book_new();
  if (back_book == NULL) return 0;
  book = ledger_book_new();
  if (book == NULL){
    ledger_book_free(back_book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    unsigned char const *note = (unsigned char const*)"note note";
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_book_set_notes(book, note);
    if (!ok) break;
    ok = ledger_io_book_write(fn,book);
    if (!ok) break;
    ok = ledger_io_book_read(fn,back_book);
    if (!ok) break;
    if (!ledger_book_is_equal(back_book,book)) break;
    result = 1;
  } while (0);
  ledger_book_free(book);
  ledger_book_free(back_book);
  return result;
}

int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  char *use_filename;
  if (argc < 2){
    fprintf(stderr,"usage: test_io_book (path_to_tmp_file)\n");
    return EXIT_FAILURE;
  }
  use_filename = argv[1];
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)(use_filename))?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
