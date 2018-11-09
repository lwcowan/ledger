/*
 * file: base/util.h
 * brief: Utility functions
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_base_util_H__
#define __Ledger_base_util_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * Call `malloc` from in this library.
 * - siz size of block to allocate
 * @return the heap-allocated block, otherwise NULL
 */
void* ledger_util_malloc(size_t siz);
/*
 * Free a block of memory.
 * - ptr pointer to block of memory to free
 */
void ledger_util_free(void* ptr);
/*
 * Duplicate a UTF-8 string.
 * - str string to duplicate
 * - ok (optional) success flag
 * @return the cloned string on success, otherwise NULL
 */
unsigned char* ledger_util_ustrdup(unsigned char const* str, int* ok);
/*
 * Compute the length of a UTF-8 string.
 * - str string to test
 * @return the length of the string
 */
size_t ledger_util_ustrlen(unsigned char const* str);
/*
 * Compare two strings.
 * - a first string to compare
 * - b second string to compare
 * @return zero if equal, negative if `a` is less than `b`,
 *   positive if `a` is greater than `b`
 */
int ledger_util_ustrcmp(unsigned char const* a, unsigned char const* b);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_ledger_H__*/

