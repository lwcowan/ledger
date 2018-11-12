
#include "book.h"
#include "../base/book.h"
#include "../base/util.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include "manifest.h"
#include <string.h>
#include <limits.h>

/*
 * Extract text from a zip archive.
 * - zip archive from which to extract
 * - name entry name
 * - ok success flag
 * @returns NULL if no entry by the given name was available (success),
 *   NULL on read fault (not success), or a pointer to the text string
 *   (success)
 */
static unsigned char* ledger_io_book_extract_text
  (struct zip_t *zip, char const* name, int *ok);
/*
 * Extract JSON from a zip archive.
 * - zip archive from which to extract
 * - name entry name
 * - ok success flag
 * @returns NULL if no entry by the given name was available (success),
 *   NULL on read fault (not success), or a pointer to the JSON structure
 *   (success)
 */
static struct cJSON* ledger_io_book_extract_json
  (struct zip_t *zip, char const* name, int *ok);
/*
 * Add JSON to a zip archive.
 * - zip archive to which to add
 * - name entry name
 * - text text string
 * @returns a success flag
 */
static int ledger_io_book_archive_text
  (struct zip_t *zip, char const* name, unsigned char const* text);
/*
 * Add JSON to a zip archive.
 * - zip archive to which to add
 * - name entry name
 * - json JSON structure
 * - ok success flag
 * @returns a success flag
 */
static int ledger_io_book_archive_json
  (struct zip_t *zip, char const* name, struct cJSON const* json);



/* BEGIN static implementation */

unsigned char* ledger_io_book_extract_text
  (struct zip_t *zip, char const* name, int* ok)
{
  int short_ok = 0;
  unsigned char* out = NULL;
  unsigned long long len;
  do {
    /* open the entry */{
      short_ok = zip_entry_open(zip, name);
      if (short_ok < 0){
        /* signal entry not present */
        *ok = 1;
        return NULL;
      }
    }
    /* get the entry length */{
      len = zip_entry_size(zip);
      if (len >= INT_MAX) break;
    }
    /* allocate space */{
      out = ledger_util_malloc(len+1u);
      if (out == NULL) break;
    }
    /* read into the buffer */{
      ssize_t progress = zip_entry_noallocread(zip, out, len);
      if (progress < 0 || progress != len){
        break;
      }
    }
    /* zero terminate */{
      out[len] = 0;
    }
    short_ok = 1;
  } while (0);
  zip_entry_close(zip);
  *ok = short_ok;
  if (!short_ok){
    ledger_util_free(out);
    return NULL;
  } else return out;
}

struct cJSON* ledger_io_book_extract_json
  (struct zip_t *zip, char const* name, int *ok)
{
  /* start with text */
  int intermediate_ok;
  unsigned char* text =
    ledger_io_book_extract_text(zip,name,&intermediate_ok);
  /* fast quit */
  if (intermediate_ok && text == NULL){
    *ok = 1;
    return NULL;
  } else if (!intermediate_ok) {
    *ok = 0;
    return NULL;
  } else {
    struct cJSON* json = cJSON_Parse((char*)text);
    ledger_util_free(text);
    *ok = (json != NULL)?1:0;
    return json;
  }
}

int ledger_io_book_archive_text
  (struct zip_t *zip, char const* name, unsigned char const* text)
{
  int ok;
  size_t len;
  len = ledger_util_ustrlen(text);
  ok = zip_entry_open(zip, name);
  if (ok < 0) return 0;
  ok = zip_entry_write(zip, text, len);
  zip_entry_close(zip);
  if (ok < 0) return 0;
  return 1;
}

int ledger_io_book_archive_json
  (struct zip_t *zip, char const* name, struct cJSON const* json)
{
  int result;
  char *printing;
  printing = cJSON_PrintUnformatted(json);
  if (printing == NULL) return 0;
  result = ledger_io_book_archive_text(zip,name,(unsigned char*)printing);
  cJSON_free(printing);
  return result;
}

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
            ledger_io_book_extract_json(active_zip,"manifest.json",&ok);
          /* require the JSON manifest */
          if (manifest_json == NULL) break;
          ok = ledger_io_manifest_parse(manifest, manifest_json);
          cJSON_Delete(manifest_json);
          if (!ok) break;
        }
        /* read top files */{
          int flags = ledger_io_manifest_get_top_flags(manifest);
          /* read the description */if (flags & LEDGER_IO_MANIFEST_DESC){
            int ok;
            unsigned char* desc =
              ledger_io_book_extract_text(active_zip, "desc.txt", &ok);
            if (desc != NULL){
              ledger_book_set_description(book, desc);
            }
            ledger_util_free(desc);
          }
          /* read the notes */if (flags & LEDGER_IO_MANIFEST_NOTES){
            int ok;
            unsigned char* notes =
              ledger_io_book_extract_text(active_zip, "notes.txt", &ok);
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
          ok = ledger_io_book_archive_json(active_zip, "manifest.json", arrangement);
          cJSON_Delete(arrangement);
          if (!ok) break;
        }
        /* put the description */{
          unsigned char const* desc = ledger_book_get_description(book);
          if (desc != NULL){
            ledger_io_book_archive_text(active_zip, "desc.txt", desc);
          }
        }
        /* put the notes */{
          unsigned char const* notes = ledger_book_get_notes(book);
          if (notes != NULL){
            ledger_io_book_archive_text(active_zip, "notes.txt", notes);
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
