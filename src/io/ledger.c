
#include "ledger.h"
#include "account.h"
#include "../base/bignum.h"
#include "../base/ledger.h"
#include "util.h"
#include "manifest.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include "../base/util.h"


/* BEGIN static implementation */



/* END   static implementation */



/* BEGIN implementation */

int ledger_io_ledger_write_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_ledger const* ledger, struct ledger_bignum* tmp_num)
{
  int result = 0;
  int const ledger_id = ledger_ledger_get_id(ledger);
  char name_buffer[100];
  /* ensure ID consistency */
  if (ledger_io_manifest_get_id(manifest) != ledger_id)
    return 0;
  else do {
    /* write description */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_DESC)
    {
      unsigned char const* desc = ledger_ledger_get_description(ledger);
      if (desc != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "ledger-%i/desc.txt", ledger_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_text(zip, name_buffer, desc))
          break;
      }
    }
    /* write name */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_NAME)
    {
      unsigned char const* name = ledger_ledger_get_name(ledger);
      if (name != NULL){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "ledger-%i/name.txt", ledger_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_text(zip, name_buffer, name))
          break;
      }
    }
    /* write the sequence number */{
      int const sequence_number = ledger_ledger_get_sequence(ledger);
      if (sequence_number >= 0){
        int ok;
        ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
              tmp_num, "ledger-%i/seq.txt", ledger_id);
        if (ok < 0) break;
        if (!ledger_io_util_archive_int(zip, name_buffer, sequence_number))
          break;
      }
    }
    /* write the sections */{
      int const count = ledger_io_manifest_get_count(manifest);
      int i;
      int account_i = 0;
      for (i = 0; i < count; ++i){
        int ok = 0;
        struct ledger_io_manifest const* sub_fest =
          ledger_io_manifest_get_c(manifest, i);
        switch (ledger_io_manifest_get_type(sub_fest)){
        case LEDGER_IO_MANIFEST_ACCOUNT:
          {
            struct ledger_account const* account =
              ledger_ledger_get_account_c(ledger, account_i);
            ok = ledger_io_account_write_items
              (zip, sub_fest, account, tmp_num, ledger_id);
            account_i += 1;
          }break;
        }
        if (!ok) break;
      }
      if (i < count) break;
    }
    result = 1;
  } while (0);
  return result;
}

int ledger_io_ledger_read_items
  ( struct zip_t* zip, struct ledger_io_manifest const* manifest,
    struct ledger_ledger* ledger, struct ledger_bignum* tmp_num)
{
  int result = 0;
  int const ledger_id = ledger_io_manifest_get_id(manifest);
  char name_buffer[100];
  do {
    /* set the ID */{
      ledger_ledger_set_id(ledger, ledger_id);
    }
    /* read description */if (
      ledger_io_manifest_get_top_flags(manifest) & LEDGER_IO_MANIFEST_DESC)
    {
      int ok;
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
            tmp_num, "ledger-%i/desc.txt", ledger_id);
      if (ok > 0){
        unsigned char* desc =
          ledger_io_util_extract_text(zip, name_buffer, &ok);
        if (desc != NULL){
          ledger_ledger_set_description(ledger, desc);
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
            tmp_num, "ledger-%i/name.txt", ledger_id);
      if (ok > 0){
        unsigned char* name =
          ledger_io_util_extract_text(zip, name_buffer, &ok);
        if (name != NULL){
          ledger_ledger_set_name(ledger, name);
        }
        ledger_util_free(name);
        if (!ok) break;
      } else break;
    }
    /* read sections */{
      int const count = ledger_io_manifest_get_count(manifest);
      int i;
      int account_i = 0;
      int account_count = 0;
      /* first pass: enumerate objects of each type */
      for (i = 0; i < count; ++i){
        struct ledger_io_manifest const* sub_fest =
          ledger_io_manifest_get_c(manifest, i);
        switch (ledger_io_manifest_get_type(sub_fest)){
        case LEDGER_IO_MANIFEST_ACCOUNT:
          account_count += 1;
          break;
        }
      }
      /* next allocate the objects needed */{
        int const ok =
          ledger_ledger_set_account_count(ledger, account_count);
        if (!ok) break;
      }
      /* second pass: process objects */
      for (i = 0; i < count; ++i){
        int ok = 0;
        struct ledger_io_manifest const* sub_fest =
          ledger_io_manifest_get_c(manifest, i);
        switch (ledger_io_manifest_get_type(sub_fest)){
        case LEDGER_IO_MANIFEST_ACCOUNT:
          {
            struct ledger_account* account =
              ledger_ledger_get_account(ledger, account_i);
            ok = ledger_io_account_read_items
              (zip, sub_fest, account, tmp_num, ledger_id);
            account_i += 1;
          }break;
        }
        if (!ok) break;
      }
      if (i < count) break;
    }
    /* read the sequence number */{
      int ok;
      ok = ledger_io_util_construct_name(name_buffer,sizeof(name_buffer),
            tmp_num, "ledger-%i/seq.txt", ledger_id);
      if (ok > 0){
        int value = ledger_io_util_extract_int(zip, name_buffer, &ok);
        if (value >= 0){
          ledger_ledger_set_sequence(ledger, value);
        }
      } else break;
    }
    result = 1;
  } while (0);
  return result;
}

/* END   implementation */
