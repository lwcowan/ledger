/*
 * file: act/select.h
 * brief: Line selection API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_act_Select_H__
#define __Ledger_act_Select_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_table;
struct ledger_table_mark;

/*
 * Select comparators.
 */
enum ledger_select_cmp {
  /* `a` equal to `b` */
  LEDGER_SELECT_EQUAL = 0,
  /* `a` less than `b` */
  LEDGER_SELECT_LESS = 1,
  /* `a` greater than `b` */
  LEDGER_SELECT_MORE = 3,
  /* `a` not equal to `b` */
  LEDGER_SELECT_NOTEQUAL = 4,
  /* `a` greater or equal to `b` */
  LEDGER_SELECT_NOTLESS = 5,
  /* `a` less than or equal to `b` */
  LEDGER_SELECT_NOTMORE = 7
};
/*
 * Comparison type.
 */
enum ledger_select_cmp_type {
  /* identifer integer */
  LEDGER_SELECT_ID = 16,
  /* big number */
  LEDGER_SELECT_BIGNUM = 32,
  /* unsigned character string  */
  LEDGER_SELECT_STRING = 48,
  /* array index integer */
  LEDGER_SELECT_INDEX = 64
};


/* select condition */
struct ledger_select_cond {
  /* comparator */
  int cmp;
  /* column index */
  int column;
  /* value to check */
  unsigned char const* value;
};

/*
 * selection callback
 * - arg callback argument
 * - m table mark
 * @return zero to continue, nonzero when done
 */
typedef int (*ledger_select_cb)(void* arg, struct ledger_table_mark const* m);

/*
 * Select certain rows from a table.
 * - table table to search
 * - arg callback argument
 * - cb callback
 * - len length of selector conditions
 * - cond condition array
 * - dir search direction (negative -> from end; positive -> from start)
 * @return negative one on error, or the first nonzero value from the
 *   callback, zero otherwise
 */
int ledger_select_by_cond
  ( struct ledger_table* t, void* arg, ledger_select_cb cb,
    int len, struct ledger_select_cond const cond[], int dir);

/*
 * Select certain rows from a table.
 * - table table to search
 * - arg callback argument
 * - cb callback
 * - len length of selector conditions
 * - cond condition array
 * - dir search direction (negative -> from end; positive -> from start)
 * @return negative one on error, or the first nonzero value from the
 *   callback, zero otherwise
 */
int ledger_select_by_cond_c
  ( struct ledger_table const* t, void* arg, ledger_select_cb cb,
    int len, struct ledger_select_cond const cond[], int dir);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_Select_H__*/
