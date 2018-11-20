
#include "entry.h"
#include "../base/entry.h"
#include "util.h"
#include "../../deps/cJSON/cJSON.h"
#include "../base/util.h"
#include <string.h>


/* BEGIN static implementation */



/* END   static implementation */



/* BEGIN implementation */


int ledger_io_entry_parse_json
  (struct ledger_entry* entry, struct cJSON const *json)
{
  int result = 0;
  do {
    if (!cJSON_IsObject(json)) break;
    /* process entry-level object */{
      struct cJSON* entry_level =
        cJSON_GetObjectItemCaseSensitive(json, "entry_id");
      if (entry_level != NULL){
        struct cJSON* entry_item;
        int ok = 1;
        cJSON_ArrayForEach(entry_item, json){
          if (strcmp(entry_item->string, "desc") == 0){
            /* description */
            if (cJSON_IsString(entry_item)){
              ok = ledger_entry_set_description
                (entry, (unsigned char*)cJSON_GetStringValue(entry_item));
            } else ok = 0;
          } else if (strcmp(entry_item->string, "name") == 0){
            /* name */
            if (cJSON_IsString(entry_item)){
              ok = ledger_entry_set_name
                (entry, (unsigned char*)cJSON_GetStringValue(entry_item));
            } else ok = 0;
          } else if (strcmp(entry_item->string, "date") == 0){
            /* date */
            if (cJSON_IsString(entry_item)){
              ok = ledger_entry_set_date
                (entry, (unsigned char*)cJSON_GetStringValue(entry_item));
            } else ok = 0;
          } else if (strcmp(entry_item->string, "entry_id") == 0){
            /* entry identifier */
            if (cJSON_IsNumber(entry_item))
              ledger_entry_set_id(entry, (int)entry_item->valuedouble);
            else ok = 0;
          }
        }
        if (!ok) break;
      } else break;
    }
    result = 1;
  } while (0);
  return result;
}

struct cJSON* ledger_io_entry_print_json(struct ledger_entry const* entry){
  int ok = 0;
  struct cJSON* json;
  json = cJSON_CreateObject();
  if (json == NULL) return NULL;
  else do {
    cJSON *sub_json;
    sub_json = cJSON_AddNumberToObject
      (json, "entry_id", ledger_entry_get_id(entry));
    if (sub_json == NULL) break;
    /* put entry name */{
      unsigned char const* name = ledger_entry_get_name(entry);
      if (name != NULL){
        sub_json = cJSON_AddStringToObject
          (json, "name", (char const*)name);
        if (sub_json == NULL) break;
      }
    }
    /* put entry description */{
      unsigned char const* desc = ledger_entry_get_description(entry);
      if (desc != NULL){
        sub_json = cJSON_AddStringToObject
          (json, "desc", (char const*)desc);
        if (sub_json == NULL) break;
      }
    }
    /* put entry date */{
      unsigned char const* date = ledger_entry_get_date(entry);
      if (date != NULL){
        sub_json = cJSON_AddStringToObject
          (json, "date", (char const*)date);
        if (sub_json == NULL) break;
      }
    }
    ok = 1;
  } while (0);
  if (!ok){
    cJSON_Delete(json);
    return NULL;
  } else return json;
}

/* END   implementation */
