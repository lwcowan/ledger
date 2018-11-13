
#include "manifest.h"
#include "../base/util.h"
#include "../base/book.h"
#include "../../deps/cJSON/cJSON.h"
#include <string.h>
#include <limits.h>

/*
 * Actualization of the manifest structure
 */
struct ledger_io_manifest {
  /* top-level flags */
  int flags;
  /* type code */
  int type_code;
  /* identification number */
  int item_id;
  /* array of arrays */
  struct ledger_io_manifest** arrays;
  /* count of arrays */
  int array_count;
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
  manifest->type_code = 0;
  manifest->arrays = NULL;
  manifest->array_count = 0;
  manifest->item_id = -1;
  return 1;
}

void ledger_io_manifest_clear(struct ledger_io_manifest* manifest){
  int i;
  manifest->flags = 0;
  manifest->type_code = 0;
  manifest->item_id = -1;
  /* use a simple recursive clearing algorithm */
  ledger_io_manifest_set_count(manifest,0);
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

int ledger_io_manifest_get_type(struct ledger_io_manifest const* m){
  return m->type_code;
}

void ledger_io_manifest_set_type(struct ledger_io_manifest* m, int typ){
  m->type_code = typ;
  return;
}

int ledger_io_manifest_get_id(struct ledger_io_manifest const* m){
  return m->item_id;
}

void ledger_io_manifest_set_id(struct ledger_io_manifest* m, int item_id){
  m->item_id = item_id;
  return;
}

int ledger_io_manifest_prepare
  (struct ledger_io_manifest* manifest, struct ledger_book const* book)
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

struct cJSON* ledger_io_manifest_print
  (struct ledger_io_manifest const* manifest)
{
  struct cJSON* out = cJSON_CreateObject();
  if (out != NULL){
    int result = 0;
    do {
      /* create top-level object */{
        struct cJSON* top_level = cJSON_AddObjectToObject(out,"top");
        if (top_level == NULL) break;
        /* add description */if (manifest->flags & LEDGER_IO_MANIFEST_DESC){
          struct cJSON* item = cJSON_AddBoolToObject(top_level,"desc",
              (manifest->flags & LEDGER_IO_MANIFEST_DESC)?1:0
            );
          if (item == NULL) break;
        }
        /* add notes */if (manifest->flags & LEDGER_IO_MANIFEST_NOTES){
          struct cJSON* item = cJSON_AddBoolToObject(top_level,"notes",
              (manifest->flags & LEDGER_IO_MANIFEST_NOTES)?1:0
            );
          if (item == NULL) break;
        }
      }
      result = 1;
    } while (0);
    if (result == 0){
      cJSON_Delete(out);
      out = NULL;
    }
  }
  return out;
}

int ledger_io_manifest_parse
  (struct ledger_io_manifest* manifest, struct cJSON const* json, int typ)
{
  ledger_io_manifest_clear(manifest);
  ledger_io_manifest_set_type(manifest, typ);
  /* process JSON */if (cJSON_IsObject(json)){
    int result = 0;
    switch (typ){
    case LEDGER_IO_MANIFEST_BOOK:
      {
        /* process top-level object */{
          struct cJSON* top_level =
            cJSON_GetObjectItemCaseSensitive(json, "top");
          if (top_level != NULL){
            struct cJSON* top_item;
            cJSON_ArrayForEach(top_item, top_level){
              if (strcmp(top_item->string, "desc") == 0){
                /* description flag */
                if (cJSON_IsTrue(top_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_DESC;
              } else if (strcmp(top_item->string, "notes") == 0){
                /* notes flag */
                if (cJSON_IsTrue(top_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_NOTES;
              }
            }
          }
        }
        result = 1;
      }break;
    }
    return result;
  } else return 0;
}

int ledger_io_manifest_get_count(struct ledger_io_manifest const* m){
  return m->array_count;
}

struct ledger_io_manifest* ledger_io_manifest_get(struct ledger_io_manifest* m, int i){
  if (i < 0 || i >= m->array_count){
    return NULL;
  } else {
    return m->arrays[i];
  }
}

struct ledger_io_manifest const* ledger_io_manifest_get_c
  (struct ledger_io_manifest const* m, int i)
{
  if (i < 0 || i >= m->array_count){
    return NULL;
  } else {
    return m->arrays[i];
  }
}

int ledger_io_manifest_set_count(struct ledger_io_manifest* m, int n){
  if (n >= INT_MAX/sizeof(struct ledger_io_manifest*)){
    return 0;
  } else if (n < 0){
    return 0;
  } else if (n == 0){
    int i;
    for (i = 0; i < m->array_count; ++i){
      ledger_io_manifest_free(m->arrays[i]);
    }
    ledger_util_free(m->arrays);
    m->arrays = NULL;
    m->array_count = 0;
    return 1;
  } else if (n < m->array_count){
    int i;
    /* allocate smaller array */
    struct ledger_io_manifest** new_array = (struct ledger_io_manifest** )
      ledger_util_malloc(n*sizeof(struct ledger_io_manifest*));
    if (new_array == NULL) return 0;
    /* move old arrays to new array */
    for (i = 0; i < n; ++i){
      new_array[i] = m->arrays[i];
    }
    /* free rest of the arrays */
    for (; i < m->array_count; ++i){
      ledger_io_manifest_free(m->arrays[i]);
    }
    ledger_util_free(m->arrays);
    m->arrays = new_array;
    m->array_count = n;
    return 1;
  } else if (n >= m->array_count){
    int save_id;
    int i;
    /* allocate larger array */
    struct ledger_io_manifest** new_array = (struct ledger_io_manifest** )
      ledger_util_malloc(n*sizeof(struct ledger_io_manifest*));
    if (new_array == NULL) return 0;
    /* make new arrays */
    for (i = m->array_count; i < n; ++i){
      new_array[i] = ledger_io_manifest_new();
      if (new_array[i] == NULL) break;
    }
    /* rollback and quit */if (i < n){
      int j;
      /* rollback */
      for (j = m->array_count; j < i; ++j){
        ledger_io_manifest_free(new_array[i]);
      }
      /* quit */
      return 0;
    }
    /* transfer old arrays */
    for (i = 0; i < m->array_count; ++i){
      new_array[i] = m->arrays[i];
    }
    /* continue */
    ledger_util_free(m->arrays);
    m->arrays = new_array;
    m->array_count = n;
    return 1;
  } else return 1 /*since n == m->array_count */;
}

/* END   implementation */
