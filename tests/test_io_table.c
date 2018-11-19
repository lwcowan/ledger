
#include "../src/base/table.h"
#include "../src/io/table.h"
#include "../src/base/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int io_table_zero_test(char const* );
static int io_table_nonzero_test(char const* );
static int io_table_bigquote_test(char const* );

struct test_struct {
  int (*fn)(char const* );
  char const* name;
};

struct test_struct test_array[] = {
  { io_table_zero_test, "i/o table zero" },
  { io_table_nonzero_test, "i/o table nonzero" },
  { io_table_bigquote_test, "i/o table with quotes" }
};


int io_table_zero_test(char const* fn){
  int result = 0;
  struct ledger_table* forward_table, * back_table;
  back_table = ledger_table_new();
  if (back_table == NULL) return 0;
  forward_table = ledger_table_new();
  if (forward_table == NULL){
    ledger_table_free(back_table);
    return 0;
  } else do {
    int ok;
    unsigned char* full_csv_text;
    full_csv_text = ledger_io_table_print_csv(forward_table);
    if (full_csv_text == NULL) break;
    ok = ledger_io_table_parse_csv(back_table, full_csv_text);
    ledger_util_free(full_csv_text);
    if (!ok) break;
    if (!ledger_table_is_equal(back_table,forward_table)) break;
    result = 1;
  } while (0);
  ledger_table_free(forward_table);
  ledger_table_free(back_table);
  return result;
}

int io_table_nonzero_test(char const* fn){
  int result = 0;
  struct ledger_table* forward_table, * back_table;
  back_table = ledger_table_new();
  if (back_table == NULL) return 0;
  forward_table = ledger_table_new();
  if (forward_table == NULL){
    ledger_table_free(back_table);
    return 0;
  } else do {
    int ok;
    unsigned char* full_csv_text;
    int column_types[3] =
      { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
    int const column_count = 3;
    /* set matched schema */{
      if (!ledger_table_set_column_types(
          forward_table, column_count, column_types))
        break;
      if (!ledger_table_set_column_types(
          back_table, column_count, column_types))
        break;
    }
    /* set rows */{
      ok = 0;
      struct ledger_table_mark* mark = NULL;
      do {
        int i, j;
        int const row_count = 3;
        mark = ledger_table_begin(forward_table);
        if (mark == NULL) break;
        /* write the tables */{
          for (j = 0; j < row_count; ++j){
            if (!ledger_table_add_row(mark)) break;
            for (i = 0; i < column_count; ++i){
              if (!ledger_table_put_string(mark, i,
                  (unsigned char*)"099.99"))
                break;
            }
            if (i < column_count) break;
            ledger_table_mark_move(mark, +1);
          }
          if (j < row_count) break;
        }
        ok = 1;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    full_csv_text = ledger_io_table_print_csv(forward_table);
    if (full_csv_text == NULL) break;
    ok = ledger_io_table_parse_csv(back_table, full_csv_text);
    ledger_util_free(full_csv_text);
    if (!ok) break;
    if (!ledger_table_is_equal(back_table,forward_table)) break;
    result = 1;
  } while (0);
  ledger_table_free(forward_table);
  ledger_table_free(back_table);
  return result;
}


int io_table_bigquote_test(char const* fn){
  int result = 0;
  struct ledger_table* forward_table, * back_table;
  back_table = ledger_table_new();
  if (back_table == NULL) return 0;
  forward_table = ledger_table_new();
  if (forward_table == NULL){
    ledger_table_free(back_table);
    return 0;
  } else do {
    int ok;
    unsigned char* full_csv_text;
    int column_types[3] =
      { LEDGER_TABLE_BIGNUM, LEDGER_TABLE_USTR, LEDGER_TABLE_ID };
    int const column_count = 3;
    /* set matched schema */{
      if (!ledger_table_set_column_types(
          forward_table, column_count, column_types))
        break;
      if (!ledger_table_set_column_types(
          back_table, column_count, column_types))
        break;
    }
    /* set rows */{
      ok = 0;
      struct ledger_table_mark* mark = NULL;
      do {
        int i, j;
        int const row_count = 3;
        mark = ledger_table_begin(forward_table);
        if (mark == NULL) break;
        /* write the tables */{
          for (j = 0; j < row_count; ++j){
            if (!ledger_table_add_row(mark)) break;
            for (i = 0; i < column_count; ++i){
              if (!ledger_table_put_string(mark, i,
                  (unsigned char*)"099.99\"\n"))
                break;
            }
            if (i < column_count) break;
            ledger_table_mark_move(mark, +1);
          }
          if (j < row_count) break;
        }
        ok = 1;
      } while (0);
      ledger_table_mark_free(mark);
    }
    if (!ok) break;
    full_csv_text = ledger_io_table_print_csv(forward_table);
    if (full_csv_text == NULL) break;
    ok = ledger_io_table_parse_csv(back_table, full_csv_text);
    ledger_util_free(full_csv_text);
    if (!ok) break;
    if (!ledger_table_is_equal(back_table,forward_table)) break;
    result = 1;
  } while (0);
  ledger_table_free(forward_table);
  ledger_table_free(back_table);
  return result;
}


int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  char *use_filename;
  if (argc < 2){
    fprintf(stderr,"usage: test_io_table (path_to_tmp_file)\n");
    return EXIT_FAILURE;
  }
  use_filename = argv[1];
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)(use_filename))?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
