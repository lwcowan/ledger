
#include "select.h"
#include "../base/book.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/table.h"
#include "../base/bignum.h"
#include "line.h"
#include <stdio.h>
#include <string.h>
#include "../act/select.h"
#include "print.h"


/* pseudo column names */
enum ledger_cli_select_columns {
  /* date */
  LEDGER_CLI_SELECT_DATE = 0,
  /* journal identifier */
  LEDGER_CLI_SELECT_JOURNAL = 1,
  /* entry descriptor identifier */
  LEDGER_CLI_SELECT_ENTRY = 2,
  /* amount */
  LEDGER_CLI_SELECT_AMOUNT = 3,
  /* check */
  LEDGER_CLI_SELECT_CHECK = 4
};

/* pseudo schema */
struct ledger_cli_select_schema {
  /* pseudo column name */
  int name;
  /* extended type code */
  int typ;
};

struct ledger_cli_select_schema ledger_cli_select_account_schema[] = {
  { LEDGER_CLI_SELECT_JOURNAL, LEDGER_TABLE_ID },
  { LEDGER_CLI_SELECT_ENTRY, LEDGER_TABLE_ID },
  { LEDGER_CLI_SELECT_AMOUNT, LEDGER_TABLE_BIGNUM },
  { LEDGER_CLI_SELECT_CHECK, LEDGER_TABLE_USTR },
  { LEDGER_CLI_SELECT_DATE, LEDGER_TABLE_USTR }
};

struct {
  int name;
  char* text;
} const ledger_cli_select_name_table[] = {
  { LEDGER_CLI_SELECT_DATE, "date" },
  { LEDGER_CLI_SELECT_CHECK, "check" },
  { LEDGER_CLI_SELECT_AMOUNT, "amount" },
  { LEDGER_CLI_SELECT_ENTRY, "entry" },
  { LEDGER_CLI_SELECT_JOURNAL, "journal" },
};

struct {
  int cmp;
  char* text;
} const ledger_cli_select_cmp_table[] = {
  { LEDGER_SELECT_EQUAL, "==" },
  { LEDGER_SELECT_LESS, "<" },
  { LEDGER_SELECT_MORE, ">" },
  { LEDGER_SELECT_NOTEQUAL, "!=" },
  { LEDGER_SELECT_NOTLESS, ">=" },
  { LEDGER_SELECT_NOTMORE, "<=" }
};

/*
 * Callback data.
 */
struct ledger_cli_select_cb {
  /* running total */
  struct ledger_bignum* sum;
  /* temporary number register */
  struct ledger_bignum* tmp;
  /* sum column */
  int sum_column;
  /* line tracker */
  struct ledger_cli_line* tracking;
};


/*
 * Convert a name text to a pseudo column name.
 * - name text to convert
 * @return a column index on success, negative otherwise
 */
static int ledger_cli_select_name_index(char const* name);

/*
 * Convert a pseudo column name to a real column.
 * - name select name
 * - schema table of type, name pairs
 * @return a schema item with non-negative name on success,
 *   with negative name otherwise
 */
static struct ledger_cli_select_schema ledger_cli_select_column_index
  (int name, struct ledger_cli_select_schema const* schema);

/*
 * Convert a comparator string to a comparator value.
 * - cmp text to convert
 * @return a comparator value on success, negative otherwise
 */
static int ledger_cli_select_cmp_index(char const* cmp);

/*
 * Initialize callback data.
 * - data structure to initialize
 * @return one on success, zero otherwise
 */
static int ledger_cli_select_cb_init(struct ledger_cli_select_cb *data);

/*
 * Clean up after some callback data.
 * - data structure to initialize
 */
static void ledger_cli_select_cb_clear(struct ledger_cli_select_cb *data);

/*
 * Selection callback.
 * - arg callback data
 * - m active mark in a table
 * @return zero on success
 */
static int ledger_cli_select_iterate
  (void* arg, struct ledger_table_mark const* m);


/* BEGIN static implementation */

int ledger_cli_select_name_index(char const* name){
  int const name_count = sizeof(ledger_cli_select_name_table)/
    sizeof(ledger_cli_select_name_table[0]);
  int name_i;
  for (name_i = 0; name_i < name_count; ++name_i){
    if (strcmp(ledger_cli_select_name_table[name_i].text, name) == 0)
      return ledger_cli_select_name_table[name_i].name;
  }
  return -1;
}

struct ledger_cli_select_schema ledger_cli_select_column_index
  (int name, struct ledger_cli_select_schema const* schema)
{
  static const struct ledger_cli_select_schema empty = {-1,-1};
  int schema_count;
  int schema_i;
  if (schema == ledger_cli_select_account_schema)
    schema_count = sizeof(ledger_cli_select_account_schema)/
      sizeof(ledger_cli_select_account_schema[0]);
  else return empty;
  for (schema_i = 0; schema_i < schema_count; ++schema_i){
    if (schema[schema_i].name == name){
      struct ledger_cli_select_schema out = {schema_i, schema[schema_i].typ};
      return out;
    }
  }
  return empty;
}

int ledger_cli_select_cmp_index(char const* cmp){
  int const cmp_count = sizeof(ledger_cli_select_cmp_table)/
    sizeof(ledger_cli_select_cmp_table[0]);
  int cmp_i;
  for (cmp_i = 0; cmp_i < cmp_count; ++cmp_i){
    if (strcmp(ledger_cli_select_cmp_table[cmp_i].text, cmp) == 0)
      return ledger_cli_select_cmp_table[cmp_i].cmp;
  }
  return -1;
}

int ledger_cli_select_cb_init(struct ledger_cli_select_cb *data){
  data->sum = ledger_bignum_new();
  if (data->sum == NULL) return 0;
  data->tmp = ledger_bignum_new();
  if (data->tmp == NULL){
    ledger_bignum_free(data->sum);
    return 0;
  } else return 1;
}

void ledger_cli_select_cb_clear(struct ledger_cli_select_cb *data){
  ledger_bignum_free(data->sum);
  ledger_bignum_free(data->tmp);
  return;
}

int ledger_cli_select_iterate
  (void* arg, struct ledger_table_mark const* m)
{
  struct ledger_cli_select_cb *const data =
    (struct ledger_cli_select_cb *)arg;
  int ok;
  do {
    if (data->sum_column >= 0){
      ok = ledger_table_fetch_bignum(m, data->sum_column, data->tmp);
      if (!ok) break;
    }
    ok = ledger_bignum_add(data->sum, data->tmp, data->sum);
    if (!ok) break;
    ok = ledger_cli_print_account_line(data->tracking, m, stderr);
    if (!ok) break;
    ok = 1;
  } while(0);
  return ok?0:1;
}

/* END   static implementation */


/* BEGIN implementation */


int ledger_cli_select(struct ledger_cli_line *tracking, int argc, char **argv){
  int argi;
  int result;
  int condition_count = 0;
  int help_flag = 0;
  struct ledger_select_cond conditions[10];
  struct ledger_book const* const book = tracking->book;
  char const* path_text = NULL;
  int direction = +1;
  /* acquire the conditions */
  if (argc < 2){
    help_flag = 1;
  } else for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-?") == 0){
      help_flag = 1;
    } else if (strcmp(argv[argi],"-r") == 0){
      /* reverse the direction */
      direction = -direction;
    } else if (strcmp(argv[argi],"-c") == 0){
      /* add a condition */
      if (condition_count >= 10){
        fprintf(stderr,"select: Too many conditions\n");
        return 1;
      } else if (++argi < argc){
        int cmp_index;
        int name_index = ledger_cli_select_name_index(argv[argi]);
        if (name_index == -1){
          fprintf(stderr,"select: Unknown column name %s\n", argv[argi]);
          return 1;
        }
        if (++argi < argc){
          cmp_index = ledger_cli_select_cmp_index(argv[argi]);
          if (cmp_index == -1){
            fprintf(stderr,"select: Unknown comparator \"%s\"\n", argv[argi]);
            return 1;
          }
        } else break;
        if (++argi < argc){
          conditions[condition_count].column = name_index;
          conditions[condition_count].cmp = cmp_index;
          conditions[condition_count].value = argv[argi];
          condition_count += 1;
        } else break;
      } else break;
    } else path_text = argv[argi];
  }
  if (help_flag){
    fputs("select: Select transaction lines.\n"
      "usage: select [-c (field) (cmp) (value) [-c ...]]\n"
      "options:\n"
      "  -c (field) (cmp) (value)\n"
      "          Add a condition for a particular field.\n"
      "          (field) field name\n"
      "          (cmp) comparator (one of ==, <, >, !=, <=, >=)\n"
      "          (value) value against which to compare\n"
      "  -r\n"
      "          Reverse the linear search direction\n"
      ,stderr);
    return 2;
  } else do {
    struct ledger_act_path new_path;
    result = 0;
    /* resolve paths */
    if (path_text != NULL){
      new_path = ledger_act_path_compute
        (book, path_text, tracking->object_path, &result);
      if (result == 0){
        fprintf(stderr,"select: Error encountered in processing path\n");
        result = 1;
      } else result = 0;
    } else new_path = tracking->object_path;
    if (result != 0) break;
    /* resolve column indices */
    switch (new_path.typ){
    case LEDGER_ACT_PATH_ACCOUNT:
      {
        int i;
        for (i = 0; i < condition_count; ++i){
          struct ledger_cli_select_schema schema_item =
            ledger_cli_select_column_index
              (conditions[i].column, ledger_cli_select_account_schema);
          if (schema_item.name == -1){
            fprintf(stderr,"select: Non-existent account field requested.\n");
            result = 1;
            break;
          } else conditions[i].column = schema_item.name;
        }
      }break;
    }
    if (result != 0) break;
    /* do the select */{
      struct ledger_table const* next_table;
      struct ledger_cli_select_cb cb_data;
      if (!ledger_cli_select_cb_init(&cb_data)){
        result = -1;
        break;
      }
      cb_data.tracking = tracking;
      switch (new_path.typ){
      case LEDGER_ACT_PATH_ACCOUNT:
        {
          struct ledger_ledger const* const ledger =
              ledger_book_get_ledger_c(book, new_path.path[0]);
          struct ledger_account const* account;
          cb_data.sum_column = ledger_cli_select_column_index
              (LEDGER_CLI_SELECT_AMOUNT, ledger_cli_select_account_schema)
            .name;
          if (ledger == NULL){
            fprintf(stderr,"Ledger unavailable.\n");
            result = 0;
            break;
          }
          account = ledger_ledger_get_account_c
            (ledger, new_path.path[1]);
          if (account == NULL){
            fprintf(stderr,"Account unavailable.\n");
            result = 0;
            break;
          }
          next_table = ledger_account_get_table_c(account);
        }break;
      default:
        {
          fprintf(stderr,"select: Unsupported object type.\n");
          result = 2;
        }break;
      }
      if (result == 0){
        result = ledger_select_by_cond_c
            ( next_table, &cb_data, &ledger_cli_select_iterate,
              condition_count, conditions, direction);
        if (!result){
          char numeric_buf[64];
          (void)ledger_bignum_get_text
            (cb_data.sum, (unsigned char*)numeric_buf, sizeof(numeric_buf), 1);
          fprintf(stderr,"balance: %s\n", numeric_buf);
        } else {
          fprintf(stderr,"select: Search terminated early.\n");
        }
      }
      ledger_cli_select_cb_clear(&cb_data);
    }
    if (result != 0) break;
  } while (0);
  return result;
}

/* END   implementation */
