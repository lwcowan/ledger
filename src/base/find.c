
#include "find.h"
#include "book.h"
#include "util.h"
#include "ledger.h"
#include "journal.h"
#include "account.h"
#include "entry.h"


/* BEGIN static implementation */


/* END   static implementation */

/* BEGIN implementation */

int ledger_find_ledger_by_name
  (struct ledger_book const* b, unsigned char const* name)
{
  /* linear search */
  int i;
  int const ledger_count = ledger_book_get_ledger_count(b);
  if (name == NULL) return -1;
  else for (i = 0; i < ledger_count; ++i){
    struct ledger_ledger const* ledger = ledger_book_get_ledger_c(b, i);
    if (ledger_util_ustrcmp(ledger_ledger_get_name(ledger), name) == 0){
      return i;
    }
  }
  return -1;
}

int ledger_find_ledger_by_id(struct ledger_book const* b, int item_id){
  /* linear search */
  int i;
  int const ledger_count = ledger_book_get_ledger_count(b);
  if (item_id < 0) return -1;
  else for (i = 0; i < ledger_count; ++i){
    struct ledger_ledger const* ledger = ledger_book_get_ledger_c(b, i);
    if (ledger_ledger_get_id(ledger) == item_id){
      return i;
    }
  }
  return -1;
}

int ledger_find_journal_by_name
  (struct ledger_book const* b, unsigned char const* name)
{
  /* linear search */
  int i;
  int const journal_count = ledger_book_get_journal_count(b);
  if (name == NULL) return -1;
  else for (i = 0; i < journal_count; ++i){
    struct ledger_journal const* journal = ledger_book_get_journal_c(b, i);
    if (ledger_util_ustrcmp(ledger_journal_get_name(journal), name) == 0){
      return i;
    }
  }
  return -1;
}

int ledger_find_journal_by_id(struct ledger_book const* b, int item_id){
  /* linear search */
  int i;
  int const journal_count = ledger_book_get_journal_count(b);
  if (item_id < 0) return -1;
  else for (i = 0; i < journal_count; ++i){
    struct ledger_journal const* journal = ledger_book_get_journal_c(b, i);
    if (ledger_journal_get_id(journal) == item_id){
      return i;
    }
  }
  return -1;
}


int ledger_find_account_by_name
  (struct ledger_ledger const* b, unsigned char const* name)
{
  /* linear search */
  int i;
  int const account_count = ledger_ledger_get_account_count(b);
  if (name == NULL) return -1;
  else for (i = 0; i < account_count; ++i){
    struct ledger_account const* account = ledger_ledger_get_account_c(b, i);
    if (ledger_util_ustrcmp(ledger_account_get_name(account), name) == 0){
      return i;
    }
  }
  return -1;
}

int ledger_find_account_by_id(struct ledger_ledger const* b, int item_id){
  /* linear search */
  int i;
  int const account_count = ledger_ledger_get_account_count(b);
  if (item_id < 0) return -1;
  else for (i = 0; i < account_count; ++i){
    struct ledger_account const* account = ledger_ledger_get_account_c(b, i);
    if (ledger_account_get_id(account) == item_id){
      return i;
    }
  }
  return -1;
}


int ledger_find_entry_by_name
  (struct ledger_journal const* b, unsigned char const* name)
{
  /* linear search */
  int i;
  int const entry_count = ledger_journal_get_entry_count(b);
  if (name == NULL) return -1;
  else for (i = 0; i < entry_count; ++i){
    struct ledger_entry const* entry = ledger_journal_get_entry_c(b, i);
    if (ledger_util_ustrcmp(ledger_entry_get_name(entry), name) == 0){
      return i;
    }
  }
  return -1;
}

int ledger_find_entry_by_id(struct ledger_journal const* b, int item_id){
  /* linear search */
  int i;
  int const entry_count = ledger_journal_get_entry_count(b);
  if (item_id < 0) return -1;
  else for (i = 0; i < entry_count; ++i){
    struct ledger_entry const* entry = ledger_journal_get_entry_c(b, i);
    if (ledger_entry_get_id(entry) == item_id){
      return i;
    }
  }
  return -1;
}

/* END   implementation */
