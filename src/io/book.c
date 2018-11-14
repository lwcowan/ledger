
#include "book.h"
#include "util.h"
#include "../base/book.h"
#include "../base/util.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include "manifest.h"
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
    struct ledger_io_manifest* manifest;
    do {
      manifest = ledger_io_manifest_new();
      if (manifest == NULL) break;
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
      }
      result = 1;
    } while (0);
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
    struct ledger_io_manifest *manifest = NULL;
    do {
      manifest = ledger_io_manifest_new();
      if (manifest == NULL) break;
      /* write top-level content */{
        int ok;
        /* compose the manifest */{
          char* printing;
          cJSON* arrangement;
          size_t len;
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
      result = 1;
    } while (0);
    ledger_io_manifest_free(manifest);
    zip_close(active_zip);
    return result;
  }
}

/* END   implementation */
