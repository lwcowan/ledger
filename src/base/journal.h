/*
 * file: base/journal.h
 * brief: Transaction journal structure
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_journal_H__
#define __Ledger_base_journal_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * brief: Journal
 */
struct ledger_journal;

/*
 * Construct a new journal.
 * @return the journal on success, otherwise NULL
 */
struct ledger_journal* ledger_journal_new(void);

/*
 * Destroy a journal.
 * - a the journal to destroy
 */
void ledger_journal_free(struct ledger_journal* a);

/*
 * Query the description of a journal.
 * - a journal to query
 * @return the description if available, otherwise NULL
 */
unsigned char const* ledger_journal_get_description
  (struct ledger_journal const* a);

/*
 * Modify the description of a journal.
 * - a journal to query
 * - desc new description
 * @return one on success, zero otherwise
 */
int ledger_journal_set_description
  (struct ledger_journal* a, unsigned char const* desc);

/*
 * Query the name of a journal.
 * - a journal to query
 * @return the name if available, otherwise NULL
 */
unsigned char const* ledger_journal_get_name
  (struct ledger_journal const* a);

/*
 * Modify the name of a journal.
 * - a journal to query
 * - name new name
 * @return one on success, zero otherwise
 */
int ledger_journal_set_name
  (struct ledger_journal* a, unsigned char const* name);

/*
 * Query the identifier of a journal.
 * - a journal to query
 * @return the identifier if available, otherwise -1
 */
int ledger_journal_get_id(struct ledger_journal const* a);

/*
 * Modify the identifier of a journal.
 * - a journal to query
 * - item_id new identifier (non-negative)
 */
void ledger_journal_set_id(struct ledger_journal* a, int item_id);

/*
 * Compare two journals for equality.
 * - a a journal
 * - b another journal
 * @return one if the journals are equal, zero otherwise
 */
int ledger_journal_is_equal
  (struct ledger_journal const* a, struct ledger_journal const* b);

/*
 * Modify a journal's transaction table.
 * - a journal to modify
 * @return the transaction table
 */
struct ledger_table* ledger_journal_get_table(struct ledger_journal* a);

/*
 * Read a journal's transaction table.
 * - a journal to read
 * @return the transaction table
 */
struct ledger_table const* ledger_journal_get_table_c
  (struct ledger_journal const* a);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_journal_H__*/
