

#include "table.h"
#include "bignum.h"
#include "util.h"

int ledger_sum_table_column
  (struct ledger_bignum* out, struct ledger_table const* table, int column)
{
  int ok = 0;
  struct ledger_table_mark* mark, * end;
  struct ledger_bignum* addend;
  mark = ledger_table_begin_c(table);
  end = ledger_table_end_c(table);
  addend = ledger_bignum_new();
  if (addend != NULL && mark != NULL && end != NULL) {
    ok = 1;
    ledger_bignum_set_long(out, 0);
    while (!ledger_table_mark_is_equal(mark, end)){
      ok = ledger_table_fetch_bignum(mark, column, addend);
      if (!ok) break;
      ok = ledger_bignum_add(out, out, addend);
      if (!ok) break;
      ledger_table_mark_move(mark, +1);
    }
  }
  ledger_bignum_free(addend);
  ledger_table_mark_free(mark);
  ledger_table_mark_free(end);
  return ok;
}
