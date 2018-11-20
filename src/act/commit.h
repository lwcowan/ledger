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

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_Commit_H__*/
