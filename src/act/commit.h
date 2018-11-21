/*
 * file: act/commit.h
 * brief: Transaction commission API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_act_Commit_H__
#define __Ledger_act_Commit_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_transaction;
struct ledger_book;

/*
 * Apply a transaction to a book.
 * - book the book into which to write
 * - act the transaction to apply
 * @return one on success, zero otherwise
 */
int ledger_commit_transaction
  (struct ledger_book* book, struct ledger_transaction* act);

/*
 * Check whether a transaction is balanced.
 * - act the transaction to apply
 * - balance (on success) one if balanced, zero if not balanced
 * @return one on successful check, zero otherwise
 */
int ledger_commit_check_balance(struct ledger_transaction* act, int *balance);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_Commit_H__*/
