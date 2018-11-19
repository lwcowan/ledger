
#include "book.h"
#include "util.h"
#include "ledger.h"
#include "../base/book.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include "manifest.h"
#include "journal.h"
#include <string.h>
#include <limits.h>



/* BEGIN static implementation */


/* END   static implementation */


/* BEGIN implementation */

int ledger_io_book_read(char const* filename, struct ledger_book* book){
  struct zip_t *active_zip = zip_open(filename, 6, 'r');
  if (active_zip == NULL){
    return 0;
  } else {
    int result = 0;
    struct ledger_bignum *tmp_num = NULL;
    struct ledger_io_manifest* manifest;
    do {
      manifest = ledger_io_manifest_new();
      if (manifest == NULL) break;
      tmp_num = ledger_bignum_new();
      if (tmp_num == NULL) break;
      if (!ledger_bignum_alloc(tmp_num, (sizeof(int)*3+2)/2, 0))
        break;
      /* read top-level content */{
        /* read the manifest */{
          int ok;
          struct cJSON* manifest_json =
            ledger_io_util_extract_json(active_zip,"manifest.json",&ok);
          /* require the JSON manifest */
          if (manifest_json == NULL) break;
          ok = ledger_io_manifest_parse(manifest, manifest_json,
              LEDGER_IO_MANIFEST_BOOK);
          cJSON_Delete(manifest_json);
          if (!ok) break;
        }
        /* read top files */{
          int flags = ledger_io_manifest_get_top_flags(manifest);
          /* read the description */if (flags & LEDGER_IO_MANIFEST_DESC){
            int ok;
            unsigned char* desc =
              ledger_io_util_extract_text(active_zip, "desc.txt", &ok);
            if (desc != NULL){
              ledger_book_set_description(book, desc);
            }
            ledger_util_free(desc);
          }
          /* read the notes */if (flags & LEDGER_IO_MANIFEST_NOTES){
            int ok;
            unsigned char* notes =
              ledger_io_util_extract_text(active_zip, "notes.txt", &ok);
            if (notes != NULL){
              ledger_book_set_notes(book, notes);
            }
            ledger_util_free(notes);
          }
        }
        /* read the chapters */{
          int const count = ledger_io_manifest_get_count(manifest);
          int i;
          int ledger_i = 0;
          int ledger_count = 0;
          int journal_i = 0;
          int journal_count = 0;
          /* first pass: enumerate objects of each type */
          for (i = 0; i < count; ++i){
            struct ledger_io_manifest const* sub_fest =
              ledger_io_manifest_get_c(manifest, i);
            switch (ledger_io_manifest_get_type(sub_fest)){
            case LEDGER_IO_MANIFEST_LEDGER:
              ledger_count += 1;
              break;
            case LEDGER_IO_MANIFEST_JOURNAL:
              journal_count += 1;
              break;
            }
          }
          /* next allocate the objects needed */{
            int const ok = ledger_book_set_ledger_count(book, ledger_count);
            if (!ok) break;
          }
          /* then allocate the journals needed */{
            int const ok = ledger_book_set_journal_count(book, journal_count);
            if (!ok) break;
          }
          /* second pass: process objects */
          for (i = 0; i < count; ++i){
            int ok = 0;
            struct ledger_io_manifest const* sub_fest =
              ledger_io_manifest_get_c(manifest, i);
            switch (ledger_io_manifest_get_type(sub_fest)){
            case LEDGER_IO_MANIFEST_LEDGER:
              {
                struct ledger_ledger* ledger =
                  ledger_book_get_ledger(book, ledger_i);
                ok = ledger_io_ledger_read_items
                  (active_zip, sub_fest, ledger, tmp_num);
                ledger_i += 1;
              }break;
            case LEDGER_IO_MANIFEST_JOURNAL:
              {
                struct ledger_journal* journal =
                  ledger_book_get_journal(book, journal_i);
                ok = ledger_io_journal_read_items
                  (active_zip, sub_fest, journal, tmp_num);
                journal_i += 1;
              }break;
            }
            if (!ok) break;
          }
          if (i < count) break;
        }
      }
      result = 1;
    } while (0);
    ledger_bignum_free(tmp_num);
    ledger_io_manifest_free(manifest);
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
    struct ledger_bignum *tmp_num = NULL;
    struct ledger_io_manifest *manifest = NULL;
    do {
      manifest = ledger_io_manifest_new();
      if (manifest == NULL) break;
      tmp_num = ledger_bignum_new();
      if (tmp_num == NULL) break;
      if (!ledger_bignum_alloc(tmp_num, (sizeof(int)*3+2)/2, 0))
        break;
      /* write top-level content */{
        int ok;
        /* compose the manifest */{
          cJSON* arrangement;
          ok = ledger_io_manifest_prepare(manifest, book);
          if (!ok) break;
          arrangement = ledger_io_manifest_print(manifest);
          if (arrangement == NULL) break;
          ok = ledger_io_util_archive_json(active_zip, "manifest.json", arrangement);
          cJSON_Delete(arrangement);
          if (!ok) break;
        }
        /* put the description */{
          unsigned char const* desc = ledger_book_get_description(book);
          if (desc != NULL){
            ledger_io_util_archive_text(active_zip, "desc.txt", desc);
          }
        }
        /* put the notes */{
          unsigned char const* notes = ledger_book_get_notes(book);
          if (notes != NULL){
            ledger_io_util_archive_text(active_zip, "notes.txt", notes);
          }
        }
      }
      /* write the chapters */{
        int const count = ledger_io_manifest_get_count(manifest);
        int i;
        int ledger_i = 0;
        int journal_i = 0;
        for (i = 0; i < count; ++i){
          int ok = 0;
          struct ledger_io_manifest const* sub_fest =
            ledger_io_manifest_get_c(manifest, i);
          switch (ledger_io_manifest_get_type(sub_fest)){
          case LEDGER_IO_MANIFEST_LEDGER:
            {
              struct ledger_ledger const* ledger =
                ledger_book_get_ledger_c(book, ledger_i);
              ok = ledger_io_ledger_write_items
                (active_zip, sub_fest, ledger, tmp_num);
              ledger_i += 1;
            }break;
          case LEDGER_IO_MANIFEST_JOURNAL:
            {
              struct ledger_journal const* journal =
                ledger_book_get_journal_c(book, journal_i);
              ok = ledger_io_journal_write_items
                (active_zip, sub_fest, journal, tmp_num);
              journal_i += 1;
            }break;
          }
          if (!ok) break;
        }
        if (i < count) break;
      }
      result = 1;
    } while (0);
    ledger_bignum_free(tmp_num);
    ledger_io_manifest_free(manifest);
    zip_close(active_zip);
    return result;
  }
}

/* END   implementation */
