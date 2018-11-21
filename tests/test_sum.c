
#include "../src/base/table.h"
#include "../src/base/sum.h"
#include "../src/base/bignum.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int simple_sum_test(void);


struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { simple_sum_test, "simple sum" }
};


int simple_sum_test(void){
  int result = 0;
  struct ledger_bignum* sum;
  struct ledger_table* table;
  sum = ledger_bignum_new();
  table = ledger_table_new();
  if (table != NULL && sum != NULL) do {
    int ok;
    int column_types[1] = { LEDGER_TABLE_BIGNUM };
    ok = ledger_table_set_column_types(table,1,column_types);
    if (!ok) break;
    /* iterate from the start */{
      int const total_rows = 10;
      int numeric = 1;
      struct ledger_table_mark* mark;
      mark = ledger_table_begin(table);
      if (mark == NULL) break;
      for (numeric = 1; numeric <= total_rows; ++numeric){
        ok = ledger_table_add_row(mark);
        if (!ok) break;
        /* set the row */{
          if (!ledger_table_put_id(mark, 0, numeric))
            break;
        }
      }
      ledger_table_mark_free(mark);
      mark = NULL;
      if (!ok) break;
    }
    if (!ledger_bignum_set_long(sum, 88))
      break;
    if (!ledger_sum_table_column(sum, table, 0))
      break;
    if (ledger_bignum_get_long(sum) != 55)
      break;
    result = 1;
  } while (0);
  ledger_bignum_free(sum);
  ledger_table_free(table);
  return result;
}





int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)())?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}
