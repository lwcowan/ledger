/*
 * file: act/arg.h
 * brief: Argument list API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_act_Arg_H__
#define __Ledger_act_Arg_H__

/*
 * Argument list
 */
struct ledger_arg_list;


/*
 * Construct a new argument list, equal to zero.
 * @return the list on success, otherwise NULL
 */
struct ledger_arg_list* ledger_arg_list_new(void);

/*
 * Destroy an argument list.
 * - l the list to destroy
 */
void ledger_arg_list_free(struct ledger_arg_list* l);


/*
 * Compare two arguments for equality.
 * - a an argument list
 * - b another argument list
 * @return one if the argument lists are equal, zero otherwise
 */
int ledger_arg_list_is_equal
  (struct ledger_arg_list const* a, struct ledger_arg_list const* b);

/*
 * Get an argument count.
 * - a list to query
 * @return the number of arguments in this list
 */
int ledger_arg_list_get_count(struct ledger_arg_list const* a);

/*
 * Set a argument count.
 * - a list to configure
 * - n number of arguments
 * @return one on success, zero otherwise
 */
int ledger_arg_list_set_count(struct ledger_arg_list* a, int n);

/*
 * Get an argument.
 * - a list to read
 * - i array index
 * @return the argument at that array index
 */
char const* ledger_arg_list_get(struct ledger_arg_list const* a, int i);

/*
 * Set an argument.
 * - a list to modify
 * - i array index
 * - s new argument text
 * @return one on success, zero otherwise
 */
int ledger_arg_list_set(struct ledger_arg_list const* a, int i, char const* s);

/*
 * Parse an argument list.
 * - a list to overwrite
 * - s new argument list text
 * @return one on success, zero otherwise
 */
int ledger_arg_list_parse(struct ledger_arg_list* a, char const* s);

/*
 * Get the direct argument array.
 * - a list to read
 * @return the argument array
 */
char const** ledger_arg_list_fetch(struct ledger_arg_list const* a);


#endif /*__Ledger_act_Arg_H__*/

