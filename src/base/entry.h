/*
 * file: base/entry.h
 * brief: Journal entry descriptor
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_entry_H__
#define __Ledger_base_entry_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * brief: Journal entry descriptor
 */
struct ledger_entry;

/*
 * Construct a new entry.
 * @return the entry on success, otherwise NULL
 */
struct ledger_entry* ledger_entry_new(void);

/*
 * Acquire a reference to an entry.
 * - a an old entry
 * @return the entry on success, otherwise NULL
 */
struct ledger_entry* ledger_entry_acquire(struct ledger_entry* a);

/*
 * Destroy an entry.
 * - a the entry to destroy
 */
void ledger_entry_free(struct ledger_entry* a);

/*
 * Query the description of an entry.
 * - a entry to query
 * @return the description if available, otherwise NULL
 */
unsigned char const* ledger_entry_get_description
  (struct ledger_entry const* a);

/*
 * Modify the description of an entry.
 * - a entry to query
 * - desc new description
 * @return one on success, zero otherwise
 */
int ledger_entry_set_description
  (struct ledger_entry* a, unsigned char const* desc);

/*
 * Query the name of an entry.
 * - a entry to query
 * @return the name if available, otherwise NULL
 */
unsigned char const* ledger_entry_get_name
  (struct ledger_entry const* a);

/*
 * Modify the name of an entry.
 * - a entry to query
 * - name new name
 * @return one on success, zero otherwise
 */
int ledger_entry_set_name
  (struct ledger_entry* a, unsigned char const* name);


/*
 * Query the date of an entry.
 * - a entry to query
 * @return the date if available, otherwise NULL
 */
unsigned char const* ledger_entry_get_date
  (struct ledger_entry const* a);

/*
 * Modify the date of an entry.
 * - a entry to query
 * - date new date
 * @return one on success, zero otherwise
 */
int ledger_entry_set_date
  (struct ledger_entry* a, unsigned char const* date);

/*
 * Query the identifier of an entry.
 * - a entry to query
 * @return the identifier if available, otherwise -1
 */
int ledger_entry_get_id(struct ledger_entry const* a);

/*
 * Modify the identifier of an entry.
 * - a entry to query
 * - item_id new identifier (non-negative)
 */
void ledger_entry_set_id(struct ledger_entry* a, int item_id);

/*
 * Compare two entrys for equality.
 * - a an entry
 * - b another entry
 * @return one if the entrys are equal, zero otherwise
 */
int ledger_entry_is_equal
  (struct ledger_entry const* a, struct ledger_entry const* b);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_entry_H__*/
