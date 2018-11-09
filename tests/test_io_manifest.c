
#include "../src/base/book.h"
#include "../src/io/manifest.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int io_manifest_test(void );
static int io_manifest_zero_test(void );
static int io_manifest_top_flag_test(void);
static int io_manifest_nonzero_test(void );

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { io_manifest_test, "i/o manifest allocate" },
  { io_manifest_top_flag_test, "i/o manifest top flag" },
  { io_manifest_nonzero_test, "i/o manifest nonzero" },
  { io_manifest_zero_test, "i/o manifest zero" }
};

int io_manifest_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    return 0;
  } else do {
    int ok;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  return result;
}
int io_manifest_top_flag_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    return 0;
  } else do {
    int ok;
    if (ledger_io_manifest_get_top_flags(manifest) != 0) break;
    ledger_io_manifest_set_top_flags(manifest, 03);
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  return result;
}
int io_manifest_zero_test(void){
  int result = 0;
  struct ledger_book* book;
  struct ledger_io_manifest* manifest;
  book = ledger_book_new();
  if (book == NULL) return 0;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    ledger_book_free(book);
    return 0;
  } else do {
    int ok;
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 0) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_book_free(book);
  return result;
}
int io_manifest_nonzero_test(void ){
  int result = 0;
  struct ledger_book* book;
  struct ledger_io_manifest* manifest;
  book = ledger_book_new();
  if (book == NULL) return 0;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    ledger_book_free(book);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    ok = ledger_book_set_notes(book, text);
    if (!ok) break;
    ok = ledger_book_set_description(book, text);
    if (!ok) break;
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    ok = ledger_book_set_description(book, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 2) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_book_free(book);
  return result;
}

int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)())?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
