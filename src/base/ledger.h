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

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_ledger_H__*/
