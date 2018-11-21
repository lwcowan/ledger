/*
 * file: act/path.h
 * brief: Object path functions
 * author: Cody Licorish (svgmovement@gmail.com)
 */
#ifndef __Ledger_act_path_H__
#define __Ledger_act_path_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct ledger_book;


enum ledger_act_path_type {
  LEDGER_ACT_PATH_BOOK = 1,
  LEDGER_ACT_PATH_LEDGER = 2,
  LEDGER_ACT_PATH_ACCOUNT = 3,
  LEDGER_ACT_PATH_JOURNAL = 4,
  LEDGER_ACT_PATH_ENTRY = 5
};

struct ledger_act_path {
  int path[2];
  int len;
  int typ;
};


/*
 * Compose a root path.
 * @return a root path
 */
struct ledger_act_path ledger_act_path_root(void);

/*
 * Construct a string representation of the given path.
 * - buf buffer to receive the etnry name
 * - len length of file name
 * - path the path to render
 * - book the book to navigate
 * @return the number of characters needed to express the path
 */
int ledger_act_path_render
  ( unsigned char* buf, int len, struct ledger_act_path path,
    struct ledger_book const* book);


/*
 * Compute an index path from a text string.
 * - book the book to navigate
 * - path_text the path
 * @return an index path on success, or a path filled with
 *   `-1` otherwise
 */
struct ledger_act_path ledger_act_path_compute
  ( struct ledger_book const* book, unsigned char const* path_text,
    struct ledger_act_path start, int *ok);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*__Ledger_act_path_H__*/
