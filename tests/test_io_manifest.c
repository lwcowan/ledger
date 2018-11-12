
#include "../src/base/book.h"
#include "../src/io/manifest.h"
#include "../deps/cJSON/cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int io_manifest_test(void );
static int io_manifest_zero_test(void );
static int io_manifest_top_flag_test(void);
static int io_manifest_nonzero_test(void );
static int io_manifest_parse_test(void);
static int io_manifest_print_test(void);
static int io_manifest_parse_zero_test(void);
static int io_manifest_print_zero_test(void);
static int new_resize_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};
struct test_struct test_array[] = {
  { io_manifest_test, "i/o manifest allocate" },
  { io_manifest_top_flag_test, "i/o manifest top flag" },
  { io_manifest_nonzero_test, "i/o manifest nonzero" },
  { io_manifest_zero_test, "i/o manifest zero" },
  { new_resize_test, "resize" },
  { io_manifest_parse_test, "parse JSON" },
  { io_manifest_print_test, "print JSON" },
  { io_manifest_parse_zero_test, "parse JSON for empty manifest" },
  { io_manifest_print_zero_test, "print JSON for empty manifest" }
};


int io_manifest_print_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json = NULL;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    int ok;
    /* set the manifest */{
      ledger_io_manifest_set_top_flags(manifest,3);
    }
    json = ledger_io_manifest_print(manifest);
    if (json == NULL) break;
    if (!cJSON_IsObject(json)) break;
    /* check top level */{
      cJSON* top_json = cJSON_GetObjectItemCaseSensitive(json, "top");
      if (top_json == NULL) break;
      if (!cJSON_IsObject(top_json)) break;
      if (cJSON_GetArraySize(top_json) != 2) break;
      /* check description */{
        cJSON* desc_json = cJSON_GetObjectItemCaseSensitive(top_json, "desc");
        if (desc_json == NULL) break;
        if (!cJSON_IsTrue(desc_json)) break;
      }
      /* check notes */{
        cJSON* notes_json =
          cJSON_GetObjectItemCaseSensitive(top_json, "notes");
        if (notes_json == NULL) break;
        if (!cJSON_IsTrue(notes_json)) break;
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}
int io_manifest_parse_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"top\":{ \"desc\": true, \"notes\": true }"
    "}";
  json = cJSON_Parse(json_text);
  if (json == NULL){
    return 0;
  }
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    int ok;
    ok = ledger_io_manifest_parse(manifest, json);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}

int io_manifest_print_zero_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json = NULL;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    int ok;
    /* set the manifest */{
      ;
    }
    json = ledger_io_manifest_print(manifest);
    if (json == NULL) break;
    if (!cJSON_IsObject(json)) break;
    /* check top level */{
      cJSON* top_json = cJSON_GetObjectItemCaseSensitive(json, "top");
      if (top_json == NULL) break;
      if (!cJSON_IsObject(top_json)) break;
      if (cJSON_GetArraySize(top_json) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}
int io_manifest_parse_zero_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"top\":{ }"
    "}";
  json = cJSON_Parse(json_text);
  if (json == NULL){
    return 0;
  }
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    int ok;
    ok = ledger_io_manifest_parse(manifest, json);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 0) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}
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
int new_resize_test(void){
  int result = 0;
  struct ledger_io_manifest* ptr;
  ptr = ledger_io_manifest_new();
  if (ptr == NULL) return 0;
  else do {
    int ok, i;
    if (ledger_io_manifest_get_count(ptr) != 0) break;
    ok = ledger_io_manifest_set_count(ptr,5);
    if (!ok) break;
    for (i = 0; i < 5; ++i){
      struct ledger_io_manifest* l = ledger_io_manifest_get(ptr,i);
      if (l != ledger_io_manifest_get(ptr,i)) break;
      if (l == NULL) break;
      if (ledger_io_manifest_get_id(l) != -1) break;
      if (ledger_io_manifest_get_type(l) != 0) break;
      ledger_io_manifest_set_id(l, i);
    }
    if (i < 5) break;
    if (ledger_io_manifest_get_c(ptr,5) != NULL) break;
    ledger_io_manifest_set_type
        (ledger_io_manifest_get(ptr,1), LEDGER_IO_MANIFEST_BOOK);
    ledger_io_manifest_set_type
        (ledger_io_manifest_get(ptr,3), LEDGER_IO_MANIFEST_LEDGER);
    ok = ledger_io_manifest_set_count(ptr,2);
    if (!ok) break;
    ok = ledger_io_manifest_set_count(ptr,4);
    if (!ok) break;
    if (ledger_io_manifest_get_count(ptr) != 4) break;
    for (i = 0; i < 10; ++i){
      struct ledger_io_manifest const* l = ledger_io_manifest_get_c(ptr,i);
      if (i < 2){
        if (l == NULL) break;
        if (ledger_io_manifest_get_id(l) != i) break;
      } else if (i < 4){
        if (l == NULL) break;
        if (ledger_io_manifest_get_id(l) != -1) break;
      } else {
        if (l != NULL) break;
      }
    }
    if (i < 10) break;
    if (ledger_io_manifest_get_type(ledger_io_manifest_get_c(ptr,1))
        != LEDGER_IO_MANIFEST_BOOK)
      break;
    if (ledger_io_manifest_get_type(ledger_io_manifest_get_c(ptr,3))
        != 0)
      break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(ptr);
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
