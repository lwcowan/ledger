/*
 * file: base/ledger.h
 * brief: Ledger structure
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_ledger_H__
#define __Ledger_base_ledger_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * brief: Ledger (holder of accounts)
 */
struct ledger_ledger;

/*
 * Construct a new ledger.
 * @return the ledger on success, otherwise NULL
 */
struct ledger_ledger* ledger_ledger_new(void);

/*
 * Acquire a reference to a ledger.
 * - a an old ledger
 * @return the ledger on success, otherwise NULL
 */
struct ledger_ledger* ledger_ledger_acquire(struct ledger_ledger* a);

/*
 * Destroy a ledger.
 * - l the ledger to destroy
 */
void ledger_ledger_free(struct ledger_ledger* l);

/*
 * Query the description of a ledger.
 * - l ledger to query
 * @return the description if available, otherwise NULL
 */
unsigned char const* ledger_ledger_get_description
  (struct ledger_ledger const* l);

/*
 * Modify the description of a ledger.
 * - l ledger to query
 * - desc new description
 * @return one on success, zero otherwise
 */
int ledger_ledger_set_description
  (struct ledger_ledger* l, unsigned char const* desc);

/*
 * Query the name of a ledger.
 * - l ledger to query
 * @return the name if available, otherwise NULL
 */
unsigned char const* ledger_ledger_get_name
  (struct ledger_ledger const* l);

/*
 * Modify the name of a ledger.
 * - l ledger to query
 * - name new name
 * @return one on success, zero otherwise
 */
int ledger_ledger_set_name
  (struct ledger_ledger* l, unsigned char const* name);

/*
 * Query the identifier of a ledger.
 * - l ledger to query
 * @return the identifier if available, otherwise -1
 */
int ledger_ledger_get_id(struct ledger_ledger const* l);

/*
 * Modify the identifier of a ledger.
 * - l ledger to query
 * - item_id new identifier (non-negative)
 */
void ledger_ledger_set_id(struct ledger_ledger* l, int item_id);

/*
 * Compare two ledgers for equality.
 * - a a ledger
 * - b another ledger
 * @return one if the ledgers are equal, zero otherwise
 */
int ledger_ledger_is_equal
  (struct ledger_ledger const* a, struct ledger_ledger const* b);


/*
 * Query the sequence identifier of a ledger.
 * - l ledger to query
 * @return the next identifier
 */
int ledger_ledger_get_sequence(struct ledger_ledger const* l);

/*
 * Modify the sequence identifier of a ledger.
 * - l ledger to query
 * - item_id new identifier (non-negative)
 */
int ledger_ledger_set_sequence(struct ledger_ledger* l, int item_id);

/*
 * Allocate a sequence identifier from a ledger.
 * - l ledger to use
 * @return a new identifier (non-negative) on success,
 *   or -1 if out of identifiers
 */
int ledger_ledger_alloc_id(struct ledger_ledger* l);


/*
 * Get an account count.
 * - l ledger to query
 * @return the number of accounts in this ledger
 */
int ledger_ledger_get_account_count(struct ledger_ledger const* l);

/*
 * Set a account count.
 * - l ledger to configure
 * - n number of ledgers
 * @return one on success, zero otherwise
 */
int ledger_ledger_set_account_count(struct ledger_ledger* l, int n);

/*
 * Get an account.
 * - l ledger to adjust
 * - i array index
 * @return the account at that array index
 */
struct ledger_account* ledger_ledger_get_account
  (struct ledger_ledger* l, int i);

/*
 * Get an account.
 * - l ledger to read
 * - i array index
 * @return the account at that array index
 */
struct ledger_account const* ledger_ledger_get_account_c
  (struct ledger_ledger const* l, int i);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_ledger_H__*/
