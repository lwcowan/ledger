
#include "account.h"
#include "../base/bignum.h"
#include "../base/account.h"
#include "util.h"
#include "manifest.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include "../base/util.h"


/* BEGIN static implementation */



/* END   static implementation */



/* BEGIN implementation */

int ledger_io_account_write_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_account const* account, struct ledger_bignum* tmp_num,
    int ledger_id)
{
  int result = 0;
  int const account_id = ledger_account_get_id(account);
  char name_buffer[100];
  /* ensure ID consistency */
  if (ledger_io_manifest_get_id(manifest) != account_id)
    return 0;
  else do {
    /* write description */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_DESC)
    {
      unsigned char const* desc = ledger_account_get_description(account);
      if (desc != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "ledger-%i/account-%i/desc.txt",
              ledger_id, account_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_text(zip, name_buffer, desc))
          break;
      }
    }
    /* write name */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_NAME)
    {
      unsigned char const* name = ledger_account_get_name(account);
      if (name != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "ledger-%i/account-%i/name.txt",
              ledger_id, account_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_text(zip, name_buffer, name))
          break;
      }
    }
    result = 1;
  } while (0);
  return result;
}

int ledger_io_account_read_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_account* account, struct ledger_bignum* tmp_num,
    int ledger_id)
{
  int result = 0;
  int const account_id = ledger_io_manifest_get_id(manifest);
  char name_buffer[100];
  do {
    /* set the ID */{
      ledger_account_set_id(account, account_id);
    }
    /* read description */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_DESC)
    {
      int ok;
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "ledger-%i/account-%i/desc.txt",
              ledger_id, account_id);
      if (ok > 0){
        unsigned char* desc =
          ledger_io_util_extract_text(zip, name_buffer, &ok);
        if (desc != NULL){
          ledger_account_set_description(account, desc);
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
              tmp_num, "ledger-%i/account-%i/name.txt",
              ledger_id, account_id);
      if (ok > 0){
        unsigned char* name =
          ledger_io_util_extract_text(zip, name_buffer, &ok);
        if (name != NULL){
          ledger_account_set_name(account, name);
        }
        ledger_util_free(name);
        if (!ok) break;
      } else break;
    }
    result = 1;
  } while (0);
  return result;
}

/* END   implementation */
