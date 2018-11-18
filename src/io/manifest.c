
#include "manifest.h"
#include "../base/util.h"
#include "../base/book.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/journal.h"
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

/*
 * Check parent-child manifest relationships.
 * - parent parent manifest
 * - child child manifest
 * @return one if the relationship provided is valid, zero otherwise
 */
static int ledger_io_manifest_check_edge
  ( struct ledger_io_manifest const* parent,
    struct ledger_io_manifest const* child);


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

int ledger_io_manifest_check_edge
  ( struct ledger_io_manifest const* parent,
    struct ledger_io_manifest const* child)
{
  if (parent == NULL || child == NULL) return 0;
  else if (parent->type_code == LEDGER_IO_MANIFEST_BOOK
  &&  child->type_code == LEDGER_IO_MANIFEST_LEDGER)
    return 1;
  else if (parent->type_code == LEDGER_IO_MANIFEST_LEDGER
  &&  child->type_code == LEDGER_IO_MANIFEST_ACCOUNT)
    return 1;
  else if (parent->type_code == LEDGER_IO_MANIFEST_BOOK
  &&  child->type_code == LEDGER_IO_MANIFEST_JOURNAL)
    return 1;
  else return 0;
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
  /* check book internals */{
    int ok;
    int i, j;
    int const ledger_count = ledger_book_get_ledger_count(book);
    int const journal_count = ledger_book_get_journal_count(book);
    if (ledger_count > INT_MAX-journal_count){
      return 0;
    }
    ok = ledger_io_manifest_set_count(manifest, ledger_count+journal_count);
    if (!ok) return 0;
    for (i = 0; i < ledger_count; ++i){
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest, i);
      struct ledger_ledger const* ledger =
        ledger_book_get_ledger_c(book, i);
      ok = ledger_io_manifest_prepare_ledger(sub_fest, ledger);
      if (!ok) break;
    }
    if (i < ledger_count) return 0;
    for (i = 0, j = ledger_count; i < journal_count; ++i, ++j){
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest, j);
      struct ledger_journal const* journal =
        ledger_book_get_journal_c(book, i);
      ok = ledger_io_manifest_prepare_journal(sub_fest, journal);
      if (!ok) break;
    }
    if (i < journal_count) return 0;
  }
  ledger_io_manifest_set_type(manifest, LEDGER_IO_MANIFEST_BOOK);
  return 1;
}

int ledger_io_manifest_prepare_ledger
  (struct ledger_io_manifest* manifest, struct ledger_ledger const* ledger)
{
  ledger_io_manifest_clear(manifest);
  /* check ledger marks */{
    int flags = 0;
    if (ledger_ledger_get_description(ledger) != NULL){
      flags |= LEDGER_IO_MANIFEST_DESC;
    }
    if (ledger_ledger_get_name(ledger) != NULL){
      flags |= LEDGER_IO_MANIFEST_NAME;
    }
    ledger_io_manifest_set_top_flags(manifest, flags);
    ledger_io_manifest_set_id(manifest,
            ledger_ledger_get_id(ledger) );
  }
  /* check ledger internals */{
    int ok;
    int i;
    int const account_count = ledger_ledger_get_account_count(ledger);
    if (account_count > INT_MAX){
      return 0;
    }
    ok = ledger_io_manifest_set_count(manifest, account_count);
    if (!ok) return 0;
    for (i = 0; i < account_count; ++i){
      struct ledger_io_manifest* sub_fest =
        ledger_io_manifest_get(manifest, i);
      struct ledger_account const* account =
        ledger_ledger_get_account_c(ledger, i);
      ok = ledger_io_manifest_prepare_account(sub_fest, account);
      if (!ok) break;
    }
    if (i < account_count) return 0;
  }
  ledger_io_manifest_set_type(manifest, LEDGER_IO_MANIFEST_LEDGER);
  return 1;
}

int ledger_io_manifest_prepare_journal
  (struct ledger_io_manifest* manifest, struct ledger_journal const* journal)
{
  ledger_io_manifest_clear(manifest);
  /* check journal marks */{
    int flags = 0;
    if (ledger_journal_get_description(journal) != NULL){
      flags |= LEDGER_IO_MANIFEST_DESC;
    }
    if (ledger_journal_get_name(journal) != NULL){
      flags |= LEDGER_IO_MANIFEST_NAME;
    }
    ledger_io_manifest_set_top_flags(manifest, flags);
    ledger_io_manifest_set_id(manifest,
            ledger_journal_get_id(journal) );
  }
  ledger_io_manifest_set_type(manifest, LEDGER_IO_MANIFEST_JOURNAL);
  return 1;
}

int ledger_io_manifest_prepare_account
  (struct ledger_io_manifest* manifest, struct ledger_account const* account)
{
  ledger_io_manifest_clear(manifest);
  /* check account marks */{
    int flags = 0;
    if (ledger_account_get_description(account) != NULL){
      flags |= LEDGER_IO_MANIFEST_DESC;
    }
    if (ledger_account_get_name(account) != NULL){
      flags |= LEDGER_IO_MANIFEST_NAME;
    }
    ledger_io_manifest_set_top_flags(manifest, flags);
    ledger_io_manifest_set_id(manifest,
            ledger_account_get_id(account) );
  }
  ledger_io_manifest_set_type(manifest, LEDGER_IO_MANIFEST_ACCOUNT);
  return 1;
}

struct cJSON* ledger_io_manifest_print
  (struct ledger_io_manifest const* manifest)
{
  struct cJSON* out = cJSON_CreateObject();
  if (out != NULL){
    int result = 0;
    switch (manifest->type_code){
    case LEDGER_IO_MANIFEST_BOOK:
      {
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
        /* create subsidiary objects */{
          int const count = ledger_io_manifest_get_count(manifest);
          if (count > 0){
            int i;
            struct cJSON* chapter_array =
              cJSON_AddArrayToObject(out,"chapters");
            if (chapter_array == NULL) break;
            for (i = 0; i < count; ++i){
              struct ledger_io_manifest const* sub_fest =
                ledger_io_manifest_get_c(manifest, i);
              struct cJSON* chapter;
              if (!ledger_io_manifest_check_edge(manifest, sub_fest))
                break;
              chapter = ledger_io_manifest_print(sub_fest);
              if (chapter == NULL) break;
              cJSON_AddItemToArray(chapter_array, chapter);
            }
            if (i < count) break;
          }
        }
        result = 1;
      }break;
    case LEDGER_IO_MANIFEST_LEDGER:
      {
        /* create ledger object */{
          struct cJSON* ledger_level = cJSON_AddObjectToObject(out,"ledger");
          if (ledger_level == NULL) break;
          /* add description */if (manifest->flags & LEDGER_IO_MANIFEST_DESC){
            struct cJSON* item = cJSON_AddBoolToObject(ledger_level,"desc",
                (manifest->flags & LEDGER_IO_MANIFEST_DESC)?1:0
              );
            if (item == NULL) break;
          }
          /* add notes */if (manifest->flags & LEDGER_IO_MANIFEST_NAME){
            struct cJSON* item = cJSON_AddBoolToObject(ledger_level,"name",
                (manifest->flags & LEDGER_IO_MANIFEST_NAME)?1:0
              );
            if (item == NULL) break;
          }
          /* add ID */{
            struct cJSON* item = cJSON_AddNumberToObject(ledger_level,"id",
                manifest->item_id);
            if (item == NULL) break;
          }
        }
        /* create subsidiary objects */{
          int const count = ledger_io_manifest_get_count(manifest);
          if (count > 0){
            int i;
            struct cJSON* section_array =
              cJSON_AddArrayToObject(out,"sections");
            if (section_array == NULL) break;
            for (i = 0; i < count; ++i){
              struct ledger_io_manifest const* sub_fest =
                ledger_io_manifest_get_c(manifest, i);
              struct cJSON* section;
              if (!ledger_io_manifest_check_edge(manifest, sub_fest))
                break;
              section = ledger_io_manifest_print(sub_fest);
              if (section == NULL) break;
              cJSON_AddItemToArray(section_array, section);
            }
            if (i < count) break;
          }
        }
        result = 1;
      }break;
    case LEDGER_IO_MANIFEST_ACCOUNT:
      {
        /* create account object */{
          struct cJSON* account_level =
            cJSON_AddObjectToObject(out,"account");
          if (account_level == NULL) break;
          /* add description */if (manifest->flags & LEDGER_IO_MANIFEST_DESC){
            struct cJSON* item = cJSON_AddBoolToObject(account_level,"desc",
                (manifest->flags & LEDGER_IO_MANIFEST_DESC)?1:0
              );
            if (item == NULL) break;
          }
          /* add notes */if (manifest->flags & LEDGER_IO_MANIFEST_NAME){
            struct cJSON* item = cJSON_AddBoolToObject(account_level,"name",
                (manifest->flags & LEDGER_IO_MANIFEST_NAME)?1:0
              );
            if (item == NULL) break;
          }
          /* add ID */{
            struct cJSON* item = cJSON_AddNumberToObject(account_level,"id",
                manifest->item_id);
            if (item == NULL) break;
          }
        }
        result = 1;
      }break;
    case LEDGER_IO_MANIFEST_JOURNAL:
      {
        /* create journal object */{
          struct cJSON* journal_level =
            cJSON_AddObjectToObject(out,"journal");
          if (journal_level == NULL) break;
          /* add description */if (manifest->flags & LEDGER_IO_MANIFEST_DESC){
            struct cJSON* item = cJSON_AddBoolToObject(journal_level,"desc",
                (manifest->flags & LEDGER_IO_MANIFEST_DESC)?1:0
              );
            if (item == NULL) break;
          }
          /* add notes */if (manifest->flags & LEDGER_IO_MANIFEST_NAME){
            struct cJSON* item = cJSON_AddBoolToObject(journal_level,"name",
                (manifest->flags & LEDGER_IO_MANIFEST_NAME)?1:0
              );
            if (item == NULL) break;
          }
          /* add ID */{
            struct cJSON* item = cJSON_AddNumberToObject(journal_level,"id",
                manifest->item_id);
            if (item == NULL) break;
          }
        }
        result = 1;
      }break;
    }
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
        /* process chapters array */{
          struct cJSON* chapters_array =
            cJSON_GetObjectItemCaseSensitive(json, "chapters");
          if (chapters_array != NULL){
            struct cJSON* chapter_item;
            int ok = 1;
            int const array_size = cJSON_GetArraySize(chapters_array);
            int active_size = 0;
            if (!ledger_io_manifest_set_count(manifest, array_size))
              break;
            cJSON_ArrayForEach(chapter_item, chapters_array){
              if (!cJSON_IsObject(chapter_item)){
                /* malformed JSON */
                ok = 0;
                break;
              }
              if (cJSON_HasObjectItem(chapter_item, "ledger") ){
                /* this is a ledger */
                struct ledger_io_manifest *next_manifest =
                    ledger_io_manifest_get(manifest, active_size);
                ok = ledger_io_manifest_parse
                  (next_manifest, chapter_item, LEDGER_IO_MANIFEST_LEDGER);
                if (!ok) break;
                active_size += 1;
              } else
              if (cJSON_HasObjectItem(chapter_item, "journal") ){
                /* this is a ledger */
                struct ledger_io_manifest *next_manifest =
                    ledger_io_manifest_get(manifest, active_size);
                ok = ledger_io_manifest_parse
                  (next_manifest, chapter_item, LEDGER_IO_MANIFEST_JOURNAL);
                if (!ok) break;
                active_size += 1;
              }
            }
            if (!ok) break;
          }
        }
        result = 1;
      }break;
    case LEDGER_IO_MANIFEST_LEDGER:
      {
        /* process ledger-level object */{
          struct cJSON* ledger_level =
            cJSON_GetObjectItemCaseSensitive(json, "ledger");
          if (ledger_level != NULL){
            struct cJSON* ledger_item;
            int ok = 1;
            cJSON_ArrayForEach(ledger_item, ledger_level){
              if (strcmp(ledger_item->string, "desc") == 0){
                /* description flag */
                if (cJSON_IsTrue(ledger_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_DESC;
              } else if (strcmp(ledger_item->string, "name") == 0){
                /* notes flag */
                if (cJSON_IsTrue(ledger_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_NAME;
              } else if (strcmp(ledger_item->string, "id") == 0){
                /* notes flag */
                if (cJSON_IsNumber(ledger_item))
                  manifest->item_id = (int)ledger_item->valuedouble;
                else ok = 0;
              }
            }
            if (!ok) break;
          }
        }
        /* process sections array */{
          struct cJSON* sections_array =
            cJSON_GetObjectItemCaseSensitive(json, "sections");
          if (sections_array != NULL){
            struct cJSON* section_item;
            int ok = 1;
            int const array_size = cJSON_GetArraySize(sections_array);
            int active_size = 0;
            if (!ledger_io_manifest_set_count(manifest, array_size))
              break;
            cJSON_ArrayForEach(section_item, sections_array){
              if (!cJSON_IsObject(section_item)){
                /* malformed JSON */
                ok = 0;
                break;
              }
              if (cJSON_HasObjectItem(section_item, "account") ){
                /* this is an account */
                struct ledger_io_manifest *next_manifest =
                    ledger_io_manifest_get(manifest, active_size);
                ok = ledger_io_manifest_parse
                  (next_manifest, section_item, LEDGER_IO_MANIFEST_ACCOUNT);
                if (!ok) break;
                active_size += 1;
              }
            }
            if (!ok) break;
          }
        }
        result = 1;
      }break;
    case LEDGER_IO_MANIFEST_ACCOUNT:
      {
        /* process account-level object */{
          struct cJSON* account_level =
            cJSON_GetObjectItemCaseSensitive(json, "account");
          if (account_level != NULL){
            struct cJSON* account_item;
            int ok = 1;
            cJSON_ArrayForEach(account_item, account_level){
              if (strcmp(account_item->string, "desc") == 0){
                /* description flag */
                if (cJSON_IsTrue(account_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_DESC;
              } else if (strcmp(account_item->string, "name") == 0){
                /* notes flag */
                if (cJSON_IsTrue(account_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_NAME;
              } else if (strcmp(account_item->string, "id") == 0){
                /* notes flag */
                if (cJSON_IsNumber(account_item))
                  manifest->item_id = (int)account_item->valuedouble;
                else ok = 0;
              }
            }
            if (!ok) break;
          }
        }
        result = 1;
      }break;
    case LEDGER_IO_MANIFEST_JOURNAL:
      {
        /* process journal-level object */{
          struct cJSON* journal_level =
            cJSON_GetObjectItemCaseSensitive(json, "journal");
          if (journal_level != NULL){
            struct cJSON* journal_item;
            int ok = 1;
            cJSON_ArrayForEach(journal_item, journal_level){
              if (strcmp(journal_item->string, "desc") == 0){
                /* description flag */
                if (cJSON_IsTrue(journal_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_DESC;
              } else if (strcmp(journal_item->string, "name") == 0){
                /* notes flag */
                if (cJSON_IsTrue(journal_item))
                  manifest->flags |= LEDGER_IO_MANIFEST_NAME;
              } else if (strcmp(journal_item->string, "id") == 0){
                /* notes flag */
                if (cJSON_IsNumber(journal_item))
                  manifest->item_id = (int)journal_item->valuedouble;
                else ok = 0;
              }
            }
            if (!ok) break;
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
