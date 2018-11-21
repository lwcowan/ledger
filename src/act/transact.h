/*
 * file: act/transact.h
 * brief: Transaction forwarding API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_act_Transact_H__
#define __Ledger_act_Transact_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * Transaction to forward
 */
struct ledger_transaction;


/*
 * Construct a new transaction.
 * @return the transaction on success, otherwise NULL
 */
struct ledger_transaction* ledger_transaction_new(void);

/*
 * Destroy a transaction.
 * - a the transaction to destroy
 */
void ledger_transaction_free(struct ledger_transaction* a);

/*
 * Query the description of a transaction.
 * - a transaction to query
 * @return the description if available, otherwise NULL
 */
unsigned char const* ledger_transaction_get_description
  (struct ledger_transaction const* a);

/*
 * Modify the description of a transaction.
 * - a transaction to query
 * - desc new description
 * @return one on success, zero otherwise
 */
int ledger_transaction_set_description
  (struct ledger_transaction* a, unsigned char const* desc);

/*
 * Query the name of a transaction.
 * - a transaction to query
 * @return the name if available, otherwise NULL
 */
unsigned char const* ledger_transaction_get_name
  (struct ledger_transaction const* a);

/*
 * Modify the name of a transaction.
 * - a transaction to query
 * - name new name
 * @return one on success, zero otherwise
 */
int ledger_transaction_set_name
  (struct ledger_transaction* a, unsigned char const* name);

/*
 * Query the date of a transaction.
 * - a transaction to query
 * @return the date if available, otherwise NULL
 */
unsigned char const* ledger_transaction_get_date
  (struct ledger_transaction const* a);

/*
 * Modify the date of a transaction.
 * - a transaction to query
 * - date new date
 * @return one on success, zero otherwise
 */
int ledger_transaction_set_date
  (struct ledger_transaction* a, unsigned char const* date);


/*
 * Compare two transactions for equality.
 * - a a transaction
 * - b another transaction
 * @return one if the transactions are equal, zero otherwise
 */
int ledger_transaction_is_equal
  (struct ledger_transaction const* a, struct ledger_transaction const* b);

/*
 * Modify a transaction's transaction table.
 * - a transaction to modify
 * @return the transaction table
 */
struct ledger_table* ledger_transaction_get_table
  (struct ledger_transaction* a);

/*
 * Read a transaction's transaction table.
 * - a transaction to read
 * @return the transaction table
 */
struct ledger_table const* ledger_transaction_get_table_c
  (struct ledger_transaction const* a);



/*
 * Query the array index of a journal.
 * - a transaction to query
 * @return the array index if available, otherwise -1
 */
int ledger_transaction_get_journal(struct ledger_transaction const* a);

/*
 * Modify the journal array index of a transaction.
 * - a transaction to query
 * - item_id new array index (non-negative)
 */
void ledger_transaction_set_journal
  (struct ledger_transaction* a, int journal_id);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_Transact_H__*/
