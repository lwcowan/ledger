
#include "manifest.h"
#include "../base/util.h"
#include "../base/book.h"

/*
 * Actualization of the manifest structure
 */
struct ledger_io_manifest {
  /* top-level flags */
  int flags;
};

/*
 * Initialize a manifest.
 * - manifest manifest to initialize
 * @return one on success, zero on failure
 */
static int ledger_io_manifest_init(struct ledger_io_manifest* manifest);
/*
 * Clear out a manifest.
 * - manifest manifest to clear
 */
static void ledger_io_manifest_clear(struct ledger_io_manifest* manifest);


/* BEGIN static implementation */

int ledger_io_manifest_init(struct ledger_io_manifest* manifest){
  manifest->flags = 0;
  return 1;
}

void ledger_io_manifest_clear(struct ledger_io_manifest* manifest){
  manifest->flags = 0;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_io_manifest* ledger_io_manifest_new(void){
  struct ledger_io_manifest* manifest = (struct ledger_io_manifest* )ledger_util_malloc
    (sizeof(struct ledger_io_manifest));
  if (manifest != NULL){
    if (!ledger_io_manifest_init(manifest)){
      ledger_util_free(manifest);
      manifest = NULL;
    }
  }
  return manifest;
}

void ledger_io_manifest_free(struct ledger_io_manifest* manifest){
  if (manifest != NULL){
    ledger_io_manifest_clear(manifest);
    ledger_util_free(manifest);
  }
}

int ledger_io_manifest_get_top_flags(struct ledger_io_manifest const* m){
  return m->flags;
}

void ledger_io_manifest_set_top_flags(struct ledger_io_manifest* m, int flags){
  m->flags = flags;
  return;
}

int ledger_io_manifest_prepare
  (struct ledger_io_manifest* manifest, struct ledger_book* book)
{
  ledger_io_manifest_clear(manifest);
  /* check top-level */{
    int flags = 0;
    if (ledger_book_get_description(book) != NULL){
      flags |= LEDGER_IO_MANIFEST_DESC;
    }
    if (ledger_book_get_notes(book) != NULL){
      flags |= LEDGER_IO_MANIFEST_NOTES;
    }
    ledger_io_manifest_set_top_flags(manifest, flags);
  }
  return 1;
}

/* END   implementation */
