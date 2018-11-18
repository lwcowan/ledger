
#include "journal.h"
#include "../base/bignum.h"
#include "../base/journal.h"
#include "util.h"
#include "table.h"
#include "manifest.h"
#include "entry.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include "../base/util.h"


/* BEGIN static implementation */



/* END   static implementation */



/* BEGIN implementation */

int ledger_io_journal_write_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_journal const* journal, struct ledger_bignum* tmp_num)
{
  int result = 0;
  int const journal_id = ledger_journal_get_id(journal);
  char name_buffer[100];
  /* ensure ID consistency */
  if (ledger_io_manifest_get_id(manifest) != journal_id)
    return 0;
  else do {
    int ok;
    /* write description */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_DESC)
    {
      unsigned char const* desc = ledger_journal_get_description(journal);
      if (desc != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "journal-%i/desc.txt",
              journal_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_text(zip, name_buffer, desc))
          break;
      }
    }
    /* write name */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_NAME)
    {
      unsigned char const* name = ledger_journal_get_name(journal);
      if (name != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "journal-%i/name.txt",
              journal_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_text(zip, name_buffer, name))
          break;
      }
    }
    /* write transaction lines */{
      struct ledger_table const* table = ledger_journal_get_table_c(journal);
      if (table != NULL){
        int ok;
        unsigned char *data_csv;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "journal-%i/lines.csv",
              journal_id);
        if (ok < 0) break;
        data_csv = ledger_io_table_print_csv(table);
        if (data_csv != NULL){
          ok = ledger_io_util_archive_text(zip, name_buffer, data_csv);
          ledger_util_free(data_csv);
          if (!ok) break;
        } else break;
      } else break;
    }
    ok = 0;
    /* write transaction entries */{
      int const count = ledger_journal_get_entry_count(journal);
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
            tmp_num, "journal-%i/entries.json",
            journal_id);
      if (ok < 0) break;
      /* put even if zero */{
        int i;
        struct cJSON* entry_array = cJSON_CreateArray();
        if (entry_array == NULL) break;
        for (i = 0; i < count; ++i){
          struct ledger_entry const* entry =
            ledger_journal_get_entry_c(journal, i);
          struct cJSON* entry_json;
          if (entry == NULL) break;
          entry_json = ledger_io_entry_print_json(entry);
          if (entry_json == NULL) break;
          cJSON_AddItemToArray(entry_array, entry_json);
        }
        if (i < count){
          cJSON_Delete(entry_array);
          break;
        } else {
          ok = ledger_io_util_archive_json(zip, name_buffer, entry_array);
          cJSON_Delete(entry_array);
        }
      }
    }
    if (!ok) break;
    result = 1;
  } while (0);
  return result;
}

int ledger_io_journal_read_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_journal* journal, struct ledger_bignum* tmp_num)
{
  int result = 0;
  int const journal_id = ledger_io_manifest_get_id(manifest);
  char name_buffer[100];
  do {
    /* set the ID */{
      ledger_journal_set_id(journal, journal_id);
    }
    /* read description */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_DESC)
    {
      int ok;
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "journal-%i/desc.txt",
              journal_id);
      if (ok > 0){
        unsigned char* desc =
          ledger_io_util_extract_text(zip, name_buffer, &ok);
        if (desc != NULL){
          ledger_journal_set_description(journal, desc);
        }
        ledger_util_free(desc);
        if (!ok) break;
      } else break;
    }
    /* read name */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_NAME)
    {
      int ok;
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "journal-%i/name.txt",
              journal_id);
      if (ok > 0){
        unsigned char* name =
          ledger_io_util_extract_text(zip, name_buffer, &ok);
        if (name != NULL){
          ledger_journal_set_name(journal, name);
        }
        ledger_util_free(name);
        if (!ok) break;
      } else break;
    }
    /* read transaction lines */{
      struct ledger_table* table = ledger_journal_get_table(journal);
      if (table != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "journal-%i/lines.csv",
              journal_id);
        if (ok > 0){
          unsigned char* data_csv =
            ledger_io_util_extract_text(zip, name_buffer, &ok);
          if (data_csv != NULL){
            ok = ledger_io_table_parse_csv(table, data_csv);
            ledger_util_free(data_csv);
          } else ok = 0;
          if (!ok) break;
        } else break;
      } else break;
    }
    /* read transaction entries */{
      int ok;
      struct cJSON* array_json;
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
            tmp_num, "journal-%i/entries.json",
            journal_id);
      if (ok > 0){
        array_json =
          ledger_io_util_extract_json(zip, name_buffer, &ok);
        if (array_json != NULL){
          if (cJSON_IsArray(array_json)) do {
            /* process entry array */{
              struct cJSON* entry_item;
              int ok = 1;
              int const array_size = cJSON_GetArraySize(array_json);
              int active_size = 0;
              if (!ledger_journal_set_entry_count(journal, array_size))
                break;
              cJSON_ArrayForEach(entry_item, array_json){
                struct ledger_entry *next_entry =
                    ledger_journal_get_entry(journal, active_size);
                ok = ledger_io_entry_parse_json(next_entry, entry_item);
                if (!ok) break;
                active_size += 1;
              }
              if (!ok) break;
            }
          } while (0);
          cJSON_Delete(array_json);
        } else ok = 0;
        if (!ok) break;
      } else break;
    }
    result = 1;
  } while (0);
  return result;
}

/* END   implementation */
