
#include "../src/base/book.h"
#include "../src/base/ledger.h"
#include "../src/base/account.h"
#include "../src/base/journal.h"
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
static int io_manifest_parse_book_test(void);
static int io_manifest_print_test(void);
static int io_manifest_parse_zero_test(void);
static int print_zero_book_test(void);
static int io_manifest_print_zero_test(void);
static int new_resize_test(void);
static int type_code_test(void);
static int print_ledger_test(void);
static int prepare_ledger_test(void);
static int parse_ledger_test(void);
static int prepare_book_with_ledger_test(void );
static int print_book_with_ledger_test(void );
static int parse_book_with_ledger_test(void);
static int print_account_test(void);
static int prepare_account_test(void);
static int parse_account_test(void);
static int prepare_ledger_with_account_test(void );
static int print_ledger_with_account_test(void );
static int parse_ledger_with_account_test(void);
static int print_journal_test(void);
static int prepare_journal_test(void);
static int parse_journal_test(void);
static int prepare_book_with_journal_test(void );
static int print_book_with_journal_test(void );
static int parse_book_with_journal_test(void);


struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { io_manifest_test, "i/o manifest allocate" },
  { io_manifest_top_flag_test, "i/o manifest top flag" },
  { io_manifest_nonzero_test, "i/o manifest nonzero" },
  { io_manifest_zero_test, "i/o manifest zero" },
  { type_code_test, "type code" },
  { new_resize_test, "resize" },
  { io_manifest_parse_test, "parse JSON" },
  { io_manifest_parse_book_test, "parse JSON book" },
  { io_manifest_print_test, "print JSON" },
  { io_manifest_parse_zero_test, "parse JSON for empty manifest" },
  { io_manifest_print_zero_test, "print JSON for empty manifest" },
  { print_zero_book_test, "print JSON for empty book manifest" },
  { print_ledger_test, "print ledger JSON" },
  { prepare_ledger_test, "prepare ledger JSON" },
  { parse_ledger_test, "parse ledger JSON" },
  { prepare_book_with_ledger_test, "prepare book with ledgers" },
  { print_book_with_ledger_test, "print book JSON with ledgers" },
  { parse_book_with_ledger_test, "parse book JSON with ledgers" },
  { print_account_test, "print account JSON" },
  { prepare_account_test, "prepare account JSON" },
  { parse_account_test, "parse account JSON" },
  { prepare_ledger_with_account_test, "prepare ledger with accounts" },
  { print_ledger_with_account_test, "print ledger JSON with accounts" },
  { parse_ledger_with_account_test, "parse ledger JSON with accounts" },
  { print_journal_test, "print journal JSON" },
  { prepare_journal_test, "prepare journal JSON" },
  { parse_journal_test, "parse journal JSON" },
  { prepare_book_with_journal_test, "prepare book with journals" },
  { print_book_with_journal_test, "print book JSON with journals" },
  { parse_book_with_journal_test, "parse book JSON with journals" }
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
    /* set the manifest */{
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_BOOK);
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
    ok = ledger_io_manifest_parse(manifest, json, 0);
    if (ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 0) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}

int io_manifest_parse_book_test(void){
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_BOOK);
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
    /* set the manifest */{
      ;
    }
    json = ledger_io_manifest_print(manifest);
    if (json != NULL) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int print_zero_book_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json = NULL;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    /* set the manifest */{
      ledger_io_manifest_set_type(manifest, LEDGER_IO_MANIFEST_BOOK);
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_BOOK);
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
    if (ledger_io_manifest_get_type(manifest) != 1) break;
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
    if (ledger_io_manifest_get_type(manifest) != 1) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    ok = ledger_book_set_description(book, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 1) break;
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

int type_code_test(void){
  int result = 0;
  struct ledger_io_manifest* ptr;
  ptr = ledger_io_manifest_new();
  if (ptr == NULL) return 0;
  else do {
    if (ledger_io_manifest_get_id(ptr) != -1) break;
    if (ledger_io_manifest_get_top_flags(ptr) != 0) break;
    if (ledger_io_manifest_get_type(ptr) != 0) break;
    ledger_io_manifest_set_type(ptr, LEDGER_IO_MANIFEST_BOOK);
    if (ledger_io_manifest_get_id(ptr) != -1) break;
    if (ledger_io_manifest_get_top_flags(ptr) != 0) break;
    if (ledger_io_manifest_get_type(ptr) != 1) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(ptr);
  return result;
}

int print_ledger_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json = NULL;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    /* set the manifest */{
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_LEDGER);
      ledger_io_manifest_set_top_flags(manifest,5);
    }
    json = ledger_io_manifest_print(manifest);
    if (json == NULL) break;
    if (!cJSON_IsObject(json)) break;
    /* check ledger level */{
      cJSON* ledger_json = cJSON_GetObjectItemCaseSensitive(json, "ledger");
      if (ledger_json == NULL) break;
      if (!cJSON_IsObject(ledger_json)) break;
      if (cJSON_GetArraySize(ledger_json) != 3) break;
      /* check description */{
        cJSON* desc_json =
          cJSON_GetObjectItemCaseSensitive(ledger_json, "desc");
        if (desc_json == NULL) break;
        if (!cJSON_IsTrue(desc_json)) break;
      }
      /* check name */{
        cJSON* name_json =
          cJSON_GetObjectItemCaseSensitive(ledger_json, "name");
        if (name_json == NULL) break;
        if (!cJSON_IsTrue(name_json)) break;
      }
      /* check ID */{
        cJSON* id_json =
          cJSON_GetObjectItemCaseSensitive(ledger_json, "id");
        if (id_json == NULL) break;
        if (!cJSON_IsNumber(id_json)) break;
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int prepare_ledger_test(void){
  int result = 0;
  struct ledger_ledger* ledger;
  struct ledger_io_manifest* manifest;
  ledger = ledger_ledger_new();
  if (ledger == NULL) return 0;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    ledger_ledger_free(ledger);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    ok = ledger_ledger_set_name(ledger, text);
    if (!ok) break;
    ok = ledger_ledger_set_description(ledger, text);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_ledger(manifest,ledger);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    if (ledger_io_manifest_get_type(manifest) != 2) break;
    ok = ledger_ledger_set_description(ledger, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_ledger(manifest,ledger);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 4) break;
    if (ledger_io_manifest_get_type(manifest) != 2) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_ledger_free(ledger);
  return result;
}

int parse_ledger_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"ledger\":{ \"desc\": true, \"id\": 7, \"name\": true }"
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_LEDGER);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 2) break;
    if (ledger_io_manifest_get_id(manifest) != 7) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}

int prepare_book_with_ledger_test(void ){
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
    ok = ledger_book_set_ledger_count(book, 2);
    if (!ok) break;
    /* modify one ledger */{
      struct ledger_ledger* ledger = ledger_book_get_ledger(book, 1);
      if (ledger == NULL) break;
      ok = ledger_ledger_set_name(ledger, text);
      if (!ok) break;
    }
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 1) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    ok = ledger_book_set_description(book, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 1) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 2) break;
    if (ledger_io_manifest_get_count(manifest) != 2) break;
    /* check first sub-fest */{
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest,0);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 0) break;
      if (ledger_io_manifest_get_type(sub_fest) != 2) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
    }
    /* check second sub-fest */{
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest,1);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 4) break;
      if (ledger_io_manifest_get_type(sub_fest) != 2) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_book_free(book);
  return result;
}

int print_book_with_ledger_test(void ){
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
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_BOOK);
      ledger_io_manifest_set_top_flags(manifest,3);
      ok = ledger_io_manifest_set_count(manifest, 2);
      if (!ok) break;
      /* initalize a ledger manifest */{
        struct ledger_io_manifest* sub_fest =
            ledger_io_manifest_get(manifest, 0);
        if (sub_fest == NULL) break;
        ledger_io_manifest_set_type(sub_fest,LEDGER_IO_MANIFEST_LEDGER);
        ledger_io_manifest_set_top_flags(sub_fest,1);
        ledger_io_manifest_set_id(sub_fest,2);
      }
      /* initalize a ledger manifest */{
        struct ledger_io_manifest* sub_fest =
            ledger_io_manifest_get(manifest, 1);
        if (sub_fest == NULL) break;
        ledger_io_manifest_set_type(sub_fest,LEDGER_IO_MANIFEST_LEDGER);
        ledger_io_manifest_set_top_flags(sub_fest,5);
        ledger_io_manifest_set_id(sub_fest,5);
      }
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
    /* check chapters level */{
      cJSON* chapter_json =
        cJSON_GetObjectItemCaseSensitive(json, "chapters");
      if (chapter_json == NULL) break;
      if (!cJSON_IsArray(chapter_json)) break;
      if (cJSON_GetArraySize(chapter_json) != 2) break;
      /* check first ledger object */{
        cJSON* ledger_json = cJSON_GetArrayItem(chapter_json, 0);
        if (ledger_json == NULL) break;
        if (!cJSON_IsObject(ledger_json)) break;
        /* check ledger top */{
          cJSON* ledger_top_json =
            cJSON_GetObjectItemCaseSensitive(ledger_json, "ledger");
          if (ledger_top_json == NULL) break;
          /* check description */{
            cJSON* desc_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "desc");
            if (desc_json == NULL) break;
            if (!cJSON_IsTrue(desc_json)) break;
          }
          /* check name */{
            cJSON* name_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "name");
            if (name_json != NULL
            &&  cJSON_IsTrue(name_json)) break;
          }
        }
      }
      /* check second ledger object */{
        cJSON* ledger_json = cJSON_GetArrayItem(chapter_json, 1);
        if (ledger_json == NULL) break;
        if (!cJSON_IsObject(ledger_json)) break;
        /* check ledger top */{
          cJSON* ledger_top_json =
            cJSON_GetObjectItemCaseSensitive(ledger_json, "ledger");
          if (ledger_top_json == NULL) break;
          /* check description */{
            cJSON* desc_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "desc");
            if (desc_json == NULL) break;
            if (!cJSON_IsTrue(desc_json)) break;
          }
          /* check name */{
            cJSON* name_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "name");
            if (name_json == NULL) break;
            if (!cJSON_IsTrue(name_json)) break;
          }
        }
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int parse_book_with_ledger_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"top\":{ \"desc\": true, \"notes\": true },"
      " \"chapters\":["
      "     {\"ledger\": { \"desc\": false, \"name\": true }}"
      "   ]"
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_BOOK);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    if (ledger_io_manifest_get_count(manifest) != 1) break;
    /* check first sub-fest */{
      struct ledger_io_manifest const* sub_fest;
      sub_fest = ledger_io_manifest_get_c(manifest, 0);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 4) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
      if (ledger_io_manifest_get_type(sub_fest) != 2) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}


int print_account_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json = NULL;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    /* set the manifest */{
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_ACCOUNT);
      ledger_io_manifest_set_top_flags(manifest,5);
    }
    json = ledger_io_manifest_print(manifest);
    if (json == NULL) break;
    if (!cJSON_IsObject(json)) break;
    /* check ledger level */{
      cJSON* ledger_json = cJSON_GetObjectItemCaseSensitive(json, "account");
      if (ledger_json == NULL) break;
      if (!cJSON_IsObject(ledger_json)) break;
      if (cJSON_GetArraySize(ledger_json) != 3) break;
      /* check description */{
        cJSON* desc_json =
          cJSON_GetObjectItemCaseSensitive(ledger_json, "desc");
        if (desc_json == NULL) break;
        if (!cJSON_IsTrue(desc_json)) break;
      }
      /* check name */{
        cJSON* name_json =
          cJSON_GetObjectItemCaseSensitive(ledger_json, "name");
        if (name_json == NULL) break;
        if (!cJSON_IsTrue(name_json)) break;
      }
      /* check ID */{
        cJSON* id_json =
          cJSON_GetObjectItemCaseSensitive(ledger_json, "id");
        if (id_json == NULL) break;
        if (!cJSON_IsNumber(id_json)) break;
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int prepare_account_test(void){
  int result = 0;
  struct ledger_account* ledger;
  struct ledger_io_manifest* manifest;
  ledger = ledger_account_new();
  if (ledger == NULL) return 0;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    ledger_account_free(ledger);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    ok = ledger_account_set_name(ledger, text);
    if (!ok) break;
    ok = ledger_account_set_description(ledger, text);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_account(manifest,ledger);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    if (ledger_io_manifest_get_type(manifest) != 3) break;
    ok = ledger_account_set_description(ledger, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_account(manifest,ledger);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 4) break;
    if (ledger_io_manifest_get_type(manifest) != 3) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_account_free(ledger);
  return result;
}

int parse_account_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"account\":{ \"desc\": true, \"id\": 7, \"name\": true }"
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_ACCOUNT);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 3) break;
    if (ledger_io_manifest_get_id(manifest) != 7) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}

int prepare_ledger_with_account_test(void ){
  int result = 0;
  struct ledger_ledger* ledger;
  struct ledger_io_manifest* manifest;
  ledger = ledger_ledger_new();
  if (ledger == NULL) return 0;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    ledger_ledger_free(ledger);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    ok = ledger_ledger_set_name(ledger, text);
    if (!ok) break;
    ok = ledger_ledger_set_description(ledger, text);
    if (!ok) break;
    ok = ledger_ledger_set_account_count(ledger, 2);
    if (!ok) break;
    /* modify one account */{
      struct ledger_account* account = ledger_ledger_get_account(ledger, 1);
      if (account == NULL) break;
      ok = ledger_account_set_name(account, text);
      if (!ok) break;
    }
    ok = ledger_io_manifest_prepare_ledger(manifest,ledger);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 2) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    ok = ledger_ledger_set_description(ledger, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_ledger(manifest,ledger);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 2) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 4) break;
    if (ledger_io_manifest_get_count(manifest) != 2) break;
    /* check first sub-fest */{
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest,0);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 0) break;
      if (ledger_io_manifest_get_type(sub_fest) != 3) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
    }
    /* check second sub-fest */{
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest,1);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 4) break;
      if (ledger_io_manifest_get_type(sub_fest) != 3) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_ledger_free(ledger);
  return result;
}

int print_ledger_with_account_test(void ){
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
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_LEDGER);
      ledger_io_manifest_set_top_flags(manifest,5);
      ok = ledger_io_manifest_set_count(manifest, 2);
      if (!ok) break;
      /* initalize a ledger manifest */{
        struct ledger_io_manifest* sub_fest =
            ledger_io_manifest_get(manifest, 0);
        if (sub_fest == NULL) break;
        ledger_io_manifest_set_type(sub_fest,LEDGER_IO_MANIFEST_ACCOUNT);
        ledger_io_manifest_set_top_flags(sub_fest,1);
        ledger_io_manifest_set_id(sub_fest,2);
      }
      /* initalize a ledger manifest */{
        struct ledger_io_manifest* sub_fest =
            ledger_io_manifest_get(manifest, 1);
        if (sub_fest == NULL) break;
        ledger_io_manifest_set_type(sub_fest,LEDGER_IO_MANIFEST_ACCOUNT);
        ledger_io_manifest_set_top_flags(sub_fest,5);
        ledger_io_manifest_set_id(sub_fest,5);
      }
    }
    json = ledger_io_manifest_print(manifest);
    if (json == NULL) break;
    if (!cJSON_IsObject(json)) break;
    /* check top level */{
      cJSON* top_json = cJSON_GetObjectItemCaseSensitive(json, "ledger");
      if (top_json == NULL) break;
      if (!cJSON_IsObject(top_json)) break;
      if (cJSON_GetArraySize(top_json) != 3) break;
      /* check description */{
        cJSON* desc_json = cJSON_GetObjectItemCaseSensitive(top_json, "desc");
        if (desc_json == NULL) break;
        if (!cJSON_IsTrue(desc_json)) break;
      }
      /* check name */{
        cJSON* name_json =
          cJSON_GetObjectItemCaseSensitive(top_json, "name");
        if (name_json == NULL) break;
        if (!cJSON_IsTrue(name_json)) break;
      }
    }
    /* check sections level */{
      cJSON* section_json =
        cJSON_GetObjectItemCaseSensitive(json, "sections");
      if (section_json == NULL) break;
      if (!cJSON_IsArray(section_json)) break;
      if (cJSON_GetArraySize(section_json) != 2) break;
      /* check first ledger object */{
        cJSON* account_json = cJSON_GetArrayItem(section_json, 0);
        if (account_json == NULL) break;
        if (!cJSON_IsObject(account_json)) break;
        /* check ledger top */{
          cJSON* account_top_json =
            cJSON_GetObjectItemCaseSensitive(account_json, "account");
          if (account_top_json == NULL) break;
          /* check description */{
            cJSON* desc_json =
              cJSON_GetObjectItemCaseSensitive(account_top_json, "desc");
            if (desc_json == NULL) break;
            if (!cJSON_IsTrue(desc_json)) break;
          }
          /* check name */{
            cJSON* name_json =
              cJSON_GetObjectItemCaseSensitive(account_top_json, "name");
            if (name_json != NULL
            &&  cJSON_IsTrue(name_json)) break;
          }
        }
      }
      /* check second ledger object */{
        cJSON* account_json = cJSON_GetArrayItem(section_json, 1);
        if (account_json == NULL) break;
        if (!cJSON_IsObject(account_json)) break;
        /* check account top */{
          cJSON* account_top_json =
            cJSON_GetObjectItemCaseSensitive(account_json, "account");
          if (account_top_json == NULL) break;
          /* check description */{
            cJSON* desc_json =
              cJSON_GetObjectItemCaseSensitive(account_top_json, "desc");
            if (desc_json == NULL) break;
            if (!cJSON_IsTrue(desc_json)) break;
          }
          /* check name */{
            cJSON* name_json =
              cJSON_GetObjectItemCaseSensitive(account_top_json, "name");
            if (name_json == NULL) break;
            if (!cJSON_IsTrue(name_json)) break;
          }
        }
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int parse_ledger_with_account_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"ledger\":{ \"desc\": true, \"name\": true },"
      " \"sections\":["
      "     {\"account\": { \"desc\": false, \"name\": true }}"
      "   ]"
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_LEDGER);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    if (ledger_io_manifest_get_count(manifest) != 1) break;
    /* check first sub-fest */{
      struct ledger_io_manifest const* sub_fest;
      sub_fest = ledger_io_manifest_get_c(manifest, 0);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 4) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
      if (ledger_io_manifest_get_type(sub_fest) != 3) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}



int print_journal_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json = NULL;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    cJSON_Delete(json);
    return 0;
  } else do {
    /* set the manifest */{
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_JOURNAL);
      ledger_io_manifest_set_top_flags(manifest,5);
    }
    json = ledger_io_manifest_print(manifest);
    if (json == NULL) break;
    if (!cJSON_IsObject(json)) break;
    /* check journal level */{
      cJSON* journal_json = cJSON_GetObjectItemCaseSensitive(json, "journal");
      if (journal_json == NULL) break;
      if (!cJSON_IsObject(journal_json)) break;
      if (cJSON_GetArraySize(journal_json) != 3) break;
      /* check description */{
        cJSON* desc_json =
          cJSON_GetObjectItemCaseSensitive(journal_json, "desc");
        if (desc_json == NULL) break;
        if (!cJSON_IsTrue(desc_json)) break;
      }
      /* check name */{
        cJSON* name_json =
          cJSON_GetObjectItemCaseSensitive(journal_json, "name");
        if (name_json == NULL) break;
        if (!cJSON_IsTrue(name_json)) break;
      }
      /* check ID */{
        cJSON* id_json =
          cJSON_GetObjectItemCaseSensitive(journal_json, "id");
        if (id_json == NULL) break;
        if (!cJSON_IsNumber(id_json)) break;
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int prepare_journal_test(void){
  int result = 0;
  struct ledger_journal* journal;
  struct ledger_io_manifest* manifest;
  journal = ledger_journal_new();
  if (journal == NULL) return 0;
  manifest = ledger_io_manifest_new();
  if (manifest == NULL){
    ledger_journal_free(journal);
    return 0;
  } else do {
    int ok;
    unsigned char const *text = (unsigned char const*)"text text";
    ok = ledger_journal_set_name(journal, text);
    if (!ok) break;
    ok = ledger_journal_set_description(journal, text);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_journal(manifest,journal);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    if (ledger_io_manifest_get_type(manifest) != 4) break;
    ok = ledger_journal_set_description(journal, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare_journal(manifest,journal);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 4) break;
    if (ledger_io_manifest_get_type(manifest) != 4) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_journal_free(journal);
  return result;
}

int parse_journal_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"journal\":{ \"desc\": true, \"id\": 7, \"name\": true }"
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_JOURNAL);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 4) break;
    if (ledger_io_manifest_get_id(manifest) != 7) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 5) break;
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
  return result;
}

int prepare_book_with_journal_test(void ){
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
    ok = ledger_book_set_journal_count(book, 2);
    if (!ok) break;
    /* modify one journal */{
      struct ledger_journal* journal = ledger_book_get_journal(book, 1);
      if (journal == NULL) break;
      ok = ledger_journal_set_name(journal, text);
      if (!ok) break;
    }
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 1) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    ok = ledger_book_set_description(book, NULL);
    if (!ok) break;
    ok = ledger_io_manifest_prepare(manifest,book);
    if (!ok) break;
    if (ledger_io_manifest_get_type(manifest) != 1) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 2) break;
    if (ledger_io_manifest_get_count(manifest) != 2) break;
    /* check first sub-fest */{
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest,0);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 0) break;
      if (ledger_io_manifest_get_type(sub_fest) != 4) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
    }
    /* check second sub-fest */{
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest,1);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 4) break;
      if (ledger_io_manifest_get_type(sub_fest) != 4) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  ledger_book_free(book);
  return result;
}

int print_book_with_journal_test(void ){
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
      ledger_io_manifest_set_type(manifest,LEDGER_IO_MANIFEST_BOOK);
      ledger_io_manifest_set_top_flags(manifest,3);
      ok = ledger_io_manifest_set_count(manifest, 2);
      if (!ok) break;
      /* initalize a journal manifest */{
        struct ledger_io_manifest* sub_fest =
            ledger_io_manifest_get(manifest, 0);
        if (sub_fest == NULL) break;
        ledger_io_manifest_set_type(sub_fest,LEDGER_IO_MANIFEST_JOURNAL);
        ledger_io_manifest_set_top_flags(sub_fest,1);
        ledger_io_manifest_set_id(sub_fest,2);
      }
      /* initalize a journal manifest */{
        struct ledger_io_manifest* sub_fest =
            ledger_io_manifest_get(manifest, 1);
        if (sub_fest == NULL) break;
        ledger_io_manifest_set_type(sub_fest,LEDGER_IO_MANIFEST_JOURNAL);
        ledger_io_manifest_set_top_flags(sub_fest,5);
        ledger_io_manifest_set_id(sub_fest,5);
      }
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
    /* check chapters level */{
      cJSON* chapter_json =
        cJSON_GetObjectItemCaseSensitive(json, "chapters");
      if (chapter_json == NULL) break;
      if (!cJSON_IsArray(chapter_json)) break;
      if (cJSON_GetArraySize(chapter_json) != 2) break;
      /* check first journal object */{
        cJSON* journal_json = cJSON_GetArrayItem(chapter_json, 0);
        if (journal_json == NULL) break;
        if (!cJSON_IsObject(journal_json)) break;
        /* check journal top */{
          cJSON* ledger_top_json =
            cJSON_GetObjectItemCaseSensitive(journal_json, "journal");
          if (ledger_top_json == NULL) break;
          /* check description */{
            cJSON* desc_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "desc");
            if (desc_json == NULL) break;
            if (!cJSON_IsTrue(desc_json)) break;
          }
          /* check name */{
            cJSON* name_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "name");
            if (name_json != NULL
            &&  cJSON_IsTrue(name_json)) break;
          }
        }
      }
      /* check second journal object */{
        cJSON* journal_json = cJSON_GetArrayItem(chapter_json, 1);
        if (journal_json == NULL) break;
        if (!cJSON_IsObject(journal_json)) break;
        /* check journal top */{
          cJSON* ledger_top_json =
            cJSON_GetObjectItemCaseSensitive(journal_json, "journal");
          if (ledger_top_json == NULL) break;
          /* check description */{
            cJSON* desc_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "desc");
            if (desc_json == NULL) break;
            if (!cJSON_IsTrue(desc_json)) break;
          }
          /* check name */{
            cJSON* name_json =
              cJSON_GetObjectItemCaseSensitive(ledger_top_json, "name");
            if (name_json == NULL) break;
            if (!cJSON_IsTrue(name_json)) break;
          }
        }
      }
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  if (json != NULL) cJSON_Delete(json);
  return result;
}

int parse_book_with_journal_test(void){
  int result = 0;
  struct ledger_io_manifest* manifest;
  struct cJSON* json;
  char const *json_text =
    "{"
      " \"top\":{ \"desc\": true, \"notes\": true },"
      " \"chapters\":["
      "     {\"journal\": { \"desc\": false, \"name\": true }}"
      "   ]"
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
    ok = ledger_io_manifest_parse(manifest, json, LEDGER_IO_MANIFEST_BOOK);
    if (!ok) break;
    if (ledger_io_manifest_get_top_flags(manifest) != 3) break;
    if (ledger_io_manifest_get_count(manifest) != 1) break;
    /* check first sub-fest */{
      struct ledger_io_manifest const* sub_fest;
      sub_fest = ledger_io_manifest_get_c(manifest, 0);
      if (sub_fest == NULL) break;
      if (ledger_io_manifest_get_top_flags(sub_fest) != 4) break;
      if (ledger_io_manifest_get_count(sub_fest) != 0) break;
      if (ledger_io_manifest_get_type(sub_fest) != 4) break;
    }
    result = 1;
  } while (0);
  ledger_io_manifest_free(manifest);
  cJSON_Delete(json);
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
