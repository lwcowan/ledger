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
 * Duplicate a UTF-8 string.
 * - str string to duplicate
 * - sz length
 * - ok (optional) success flag
 * @return the cloned string on success, otherwise NULL
 */
unsigned char* ledger_util_ustrndup
  (unsigned char const* str, size_t sz, int* ok);

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

/*
 * Compare two strings.
 * - a first string to compare
 * - b second string to compare
 * - sz maximum length to compare
 * @return zero if equal, negative if `a` is less than `b`,
 *   positive if `a` is greater than `b`
 */
int ledger_util_ustrncmp
  (unsigned char const* a, unsigned char const* b, size_t sz);

/*
 * Compare a string of bytes to zero.
 * - a string to compare
 * - siz length of string in bytes
 * @return one if all zero, zero otherwise
 */
int ledger_util_uiszero(unsigned char const* a, size_t siz);

/*
 * Convert an integer to a string.
 * - n the integer to convert
 * - buf buffer to receive the string
 * - siz size of buffer in bytes
 * - want_plus nonzero if a plus sign is desired for positive integers
 * @return the number of bytes needed to perform the conversion,
 *   not including a NUL terminator
 */
size_t ledger_util_itoa(int n, unsigned char* buf, size_t siz, int want_plus);

/*
 * Convert a string to an integer.
 * - str the string to convert
 * @return the number
 */
int ledger_util_atoi(unsigned char const* str);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_base_ledger_H__*/

