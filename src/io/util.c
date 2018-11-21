
#include "util.h"
#include "../base/util.h"
#include "../base/bignum.h"
#include "../../deps/zip/src/zip.h"
#include "../../deps/cJSON/cJSON.h"
#include <string.h>
#include <limits.h>
#include <stdarg.h>


unsigned char* ledger_io_util_extract_text
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

struct cJSON* ledger_io_util_extract_json
  (struct zip_t *zip, char const* name, int *ok)
{
  /* start with text */
  int intermediate_ok;
  unsigned char* text =
    ledger_io_util_extract_text(zip,name,&intermediate_ok);
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

int ledger_io_util_extract_int
  (struct zip_t *zip, char const* name, int *ok)
{
  /* start with text */
  int intermediate_ok;
  unsigned char* text =
    ledger_io_util_extract_text(zip,name,&intermediate_ok);
  /* fast quit */
  if (intermediate_ok && text == NULL){
    *ok = 1;
    return 0;
  } else if (!intermediate_ok) {
    *ok = 0;
    return -1;
  } else {
    int value = ledger_util_atoi(text);
    ledger_util_free(text);
    *ok = 1;
    return value;
  }
}

int ledger_io_util_archive_text
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

int ledger_io_util_archive_json
  (struct zip_t *zip, char const* name, struct cJSON const* json)
{
  int result;
  char *printing;
  printing = cJSON_PrintUnformatted(json);
  if (printing == NULL) return 0;
  result = ledger_io_util_archive_text(zip,name,(unsigned char*)printing);
  cJSON_free(printing);
  return result;
}

int ledger_io_util_archive_int
  (struct zip_t *zip, char const* name, int value)
{
  int result;
  unsigned char printing[1+sizeof(int)*CHAR_BIT/2];
  ledger_util_itoa(value, printing, sizeof(printing), 0);
  result = ledger_io_util_archive_text(zip,name,printing);
  return result;
}

int ledger_io_util_construct_name
  (char* buf, int len, struct ledger_bignum* tmp_num, char const* format, ...)
{
  int write_point = 0;
  char const* text_ptr;
  va_list ap;
  va_start(ap,format);
  for (text_ptr = format; *text_ptr; ++text_ptr){
    if (*text_ptr == '%'){
      text_ptr += 1;
      if (*text_ptr == 0) break;
      else switch (*text_ptr){
      case 'i':
        {
          int numeric_length;
          int apt_length;
          int n = va_arg(ap,int);
          if (!ledger_bignum_set_long(tmp_num, n))
            return -1;
          numeric_length = ledger_bignum_get_text(tmp_num, NULL, 0, 0);
          if (numeric_length >= len-write_point){
            apt_length = len-write_point;
          } else apt_length = numeric_length+1;
          if (apt_length > 0) {
            ledger_bignum_get_text
              (tmp_num, ((unsigned char*)buf)+write_point, apt_length, 0);
          }
          write_point += numeric_length;
        }break;
      default:
        /* ignore and */break;
      }
    } else {
      if (write_point < len) buf[write_point] = *text_ptr;
      write_point += 1;
    }
  }
  va_end(ap);
  if (write_point < len) buf[write_point] = 0;
  else if (len > 0) buf[len-1] = 0;
  return write_point;
}
