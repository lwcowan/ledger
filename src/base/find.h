/*
 * file: base/find.h
 * brief: Array search API
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_find_H__
#define __Ledger_base_find_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_ledger;
struct ledger_journal;
struct ledger_book;

/*
 * Find a ledger by name.
 * - b book to read
 * - name ledger name
 * @return the index of the ledger, or -1 if not found
 */
int ledger_find_ledger_by_name
  (struct ledger_book const* b, unsigned char const* name);

/*
 * Find a ledger by identifier.
 * - b book to read
 * - name ledger identifier
 * @return the index of the ledger, or -1 if not found
 */
int ledger_find_ledger_by_id(struct ledger_book const* b, int item_id);



/*
 * Find a journal by name.
 * - b book to read
 * - name journal name
 * @return the index of the journal, or -1 if not found
 */
int ledger_find_journal_by_name
  (struct ledger_book const* b, unsigned char const* name);

/*
 * Find a journal by identifier.
 * - b book to read
 * - name journal identifier
 * @return the index of the journal, or -1 if not found
 */
int ledger_find_journal_by_id(struct ledger_book const* b, int item_id);



/*
 * Find an account by name.
 * - b ledger to read
 * - name account name
 * @return the index of the account, or -1 if not found
 */
int ledger_find_account_by_name
  (struct ledger_ledger const* b, unsigned char const* name);

/*
 * Find an account by identifier.
 * - b ledger to read
 * - name account identifier
 * @return the index of the account, or -1 if not found
 */
int ledger_find_account_by_id(struct ledger_ledger const* b, int item_id);



/*
 * Find an entry by name.
 * - b journal to read
 * - name entry name
 * @return the index of the entry, or -1 if not found
 */
int ledger_find_entry_by_name
  (struct ledger_journal const* b, unsigned char const* name);

/*
 * Find an entry by identifier.
 * - b journal to read
 * - name entry identifier
 * @return the index of the entry, or -1 if not found
 */
int ledger_find_entry_by_id(struct ledger_journal const* b, int item_id);


#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_find_H__*/
