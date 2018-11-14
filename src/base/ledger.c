
#include "ledger.h"
#include "util.h"
#include "account.h"
#include <limits.h>

/*
 * Actualization of the ledger structure
 */
struct ledger_ledger {
  unsigned char *name;
  unsigned char *description;
  int item_id;
  /*
   * brief: account count
   */
  int account_count;
  /*
   * brief: array of accounts
   */
  struct ledger_account** accounts;
  /*
   * next id to use
   */
  int sequence_id;
};

/*
 * Initialize a ledger.
 * - l ledger to initialize
 * @return one on success, zero on failure
 */
static int ledger_ledger_init(struct ledger_ledger* l);

/*
 * Clear out a ledger.
 * - l ledger to clear
 */
static void ledger_ledger_clear(struct ledger_ledger* l);


/* BEGIN static implementation */

int ledger_ledger_init(struct ledger_ledger* l){
  l->description = NULL;
  l->name = NULL;
  l->item_id = -1;
  l->sequence_id = 0;
  l->accounts = NULL;
  l->account_count = 0;
  return 1;
}

void ledger_ledger_clear(struct ledger_ledger* l){
  ledger_ledger_set_account_count(l,0);
  ledger_util_free(l->description);
  l->description = NULL;
  ledger_util_free(l->name);
  l->description = NULL;
  l->item_id = -1;
  l->sequence_id = 0;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

struct ledger_ledger* ledger_ledger_new(void){
  struct ledger_ledger* l = (struct ledger_ledger* )ledger_util_malloc
    (sizeof(struct ledger_ledger));
  if (l != NULL){
    if (!ledger_ledger_init(l)){
      ledger_util_free(l);
      l = NULL;
    }
  }
  return l;
}

void ledger_ledger_free(struct ledger_ledger* l){
  if (l != NULL){
    ledger_ledger_clear(l);
    ledger_util_free(l);
  }
}

unsigned char const* ledger_ledger_get_description
  (struct ledger_ledger const* l)
{
  return l->description;
}

int ledger_ledger_set_description
  (struct ledger_ledger* l, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(l->description);
    l->description = new_desc;
    return 1;
  } else return 0;
}

unsigned char const* ledger_ledger_get_name
  (struct ledger_ledger const* l)
{
  return l->name;
}

int ledger_ledger_set_name
  (struct ledger_ledger* l, unsigned char const* desc)
{
  int ok;
  unsigned char* new_desc = ledger_util_ustrdup(desc,&ok);
  if (ok){
    ledger_util_free(l->name);
    l->name = new_desc;
    return 1;
  } else return 0;
}

int ledger_ledger_get_id(struct ledger_ledger const* l){
  return l->item_id;
}

void ledger_ledger_set_id(struct ledger_ledger* l, int item_id){
  if (item_id < 0){
    l->item_id = -1;
  } else {
    l->item_id = item_id;
  }
  return;
}

int ledger_ledger_is_equal
  (struct ledger_ledger const* a, struct ledger_ledger const* b)
{
  /* trivial ledgers */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare top-level features */{
    if (a->item_id != b->item_id)
      return 0;
    if (ledger_util_ustrcmp(a->name, b->name) != 0)
      return 0;
    if (ledger_util_ustrcmp(a->description, b->description) != 0)
      return 0;
  }
  /* compare accounts */{
    int i;
    if (a->account_count != b->account_count) return 0;
    else for (i = 0; i < a->account_count; ++i){
      if (!ledger_account_is_equal(a->accounts[i], b->accounts[i]))
        break;
    }
    if (i < a->account_count) return 0;
  }
  return 1;
}


int ledger_ledger_get_sequence(struct ledger_ledger const* l){
  return l->sequence_id;
}

int ledger_ledger_set_sequence(struct ledger_ledger* l, int item_id){
  if (item_id < 0) return 0;
  l->sequence_id = item_id;
  return 1;
}

int ledger_ledger_alloc_id(struct ledger_ledger* l){
  if (l->sequence_id < INT_MAX){
    int out;
    out = l->sequence_id;
    l->sequence_id += 1;
    return out;
  } else return -1;
}

int ledger_ledger_get_account_count(struct ledger_ledger const* l){
  return l->account_count;
}
struct ledger_account* ledger_ledger_get_account
  (struct ledger_ledger* l, int i)
{
  if (i < 0 || i >= l->account_count){
    return NULL;
  } else {
    return l->accounts[i];
  }
}
struct ledger_account const* ledger_ledger_get_account_c
  (struct ledger_ledger const* l, int i)
{
  if (i < 0 || i >= l->account_count){
    return NULL;
  } else {
    return l->accounts[i];
  }
}
int ledger_ledger_set_account_count(struct ledger_ledger* l, int n){
  if (n >= INT_MAX/sizeof(struct ledger_account*)){
    return 0;
  } else if (n < 0){
    return 0;
  } else if (n == 0){
    int i;
    for (i = 0; i < l->account_count; ++i){
      ledger_account_free(l->accounts[i]);
    }
    ledger_util_free(l->accounts);
    l->accounts = NULL;
    l->account_count = 0;
    return 1;
  } else if (n < l->account_count){
    int i;
    /* allocate smaller array */
    struct ledger_account** new_array = (struct ledger_account** )
      ledger_util_malloc(n*sizeof(struct ledger_account*));
    if (new_array == NULL) return 0;
    /* move old accounts to new array */
    for (i = 0; i < n; ++i){
      new_array[i] = l->accounts[i];
    }
    /* free rest of the accounts */
    for (; i < l->account_count; ++i){
      ledger_account_free(l->accounts[i]);
    }
    ledger_util_free(l->accounts);
    l->accounts = new_array;
    l->account_count = n;
    return 1;
  } else if (n >= l->account_count){
    int save_id;
    int i;
    /* allocate larger array */
    struct ledger_account** new_array = (struct ledger_account** )
      ledger_util_malloc(n*sizeof(struct ledger_account*));
    if (new_array == NULL) return 0;
    /* save the sequence number in case of rollback */
    save_id = l->sequence_id;
    /* make new accounts */
    for (i = l->account_count; i < n; ++i){
      int next_id = ledger_ledger_alloc_id(l);
      if (next_id == -1) break;
      new_array[i] = ledger_account_new();
      if (new_array[i] == NULL) break;
      ledger_account_set_id(new_array[i], next_id);
    }
    /* rollback and quit */if (i < n){
      int j;
      /* rollback */
      for (j = l->account_count; j < i; ++j){
        ledger_account_free(new_array[i]);
      }
      l->sequence_id = save_id;
      /* quit */
      return 0;
    }
    /* transfer old accounts */
    for (i = 0; i < l->account_count; ++i){
      new_array[i] = l->accounts[i];
    }
    /* continue */
    ledger_util_free(l->accounts);
    l->accounts = new_array;
    l->account_count = n;
    return 1;
  } else return 1 /*since n == l->account_count */;
}



/* END   implementation */
