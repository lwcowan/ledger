/*
 * file: base/account.h
 * brief: Account structure
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_account_H__
#define __Ledger_base_account_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * brief: Account
 */
struct ledger_account;

/*
 * Construct a new account.
 * @return the account on success, otherwise NULL
 */
struct ledger_account* ledger_account_new(void);

/*
 * Acquire a reference to an account.
 * - a an old account
 * @return the account on success, otherwise NULL
 */
struct ledger_account* ledger_account_acquire(struct ledger_account* a);

/*
 * Destroy an account.
 * - a the account to destroy
 */
void ledger_account_free(struct ledger_account* a);

/*
 * Query the description of an account.
 * - a account to query
 * @return the description if available, otherwise NULL
 */
unsigned char const* ledger_account_get_description
  (struct ledger_account const* a);

/*
 * Modify the description of an account.
 * - a account to query
 * - desc new description
 * @return one on success, zero otherwise
 */
int ledger_account_set_description
  (struct ledger_account* a, unsigned char const* desc);

/*
 * Query the name of an account.
 * - a account to query
 * @return the name if available, otherwise NULL
 */
unsigned char const* ledger_account_get_name
  (struct ledger_account const* a);

/*
 * Modify the name of an account.
 * - a account to query
 * - name new name
 * @return one on success, zero otherwise
 */
int ledger_account_set_name
  (struct ledger_account* a, unsigned char const* name);

/*
 * Query the identifier of an account.
 * - a account to query
 * @return the identifier if available, otherwise -1
 */
int ledger_account_get_id(struct ledger_account const* a);

/*
 * Modify the identifier of an account.
 * - a account to query
 * - item_id new identifier (non-negative)
 */
void ledger_account_set_id(struct ledger_account* a, int item_id);

/*
 * Compare two accounts for equality.
 * - a an account
 * - b another account
 * @return one if the accounts are equal, zero otherwise
 */
int ledger_account_is_equal
  (struct ledger_account const* a, struct ledger_account const* b);

/*
 * Modify an account's transaction table. The user should acquire a
 *     reference to the table by calling `ledger_table_acquire` after
 *     this function.
 * - a account to modify
 * @return the transaction table
 */
struct ledger_table* ledger_account_get_table(struct ledger_account* a);

/*
 * Read an account's transaction table.
 * - a account to read
 * @return the transaction table
 */
struct ledger_table const* ledger_account_get_table_c
  (struct ledger_account const* a);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_account_H__*/
