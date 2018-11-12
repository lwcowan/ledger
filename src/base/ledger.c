
#include "ledger.h"
#include "util.h"

/*
 * Actualization of the ledger structure
 */
struct ledger_ledger {
  unsigned char *name;
  unsigned char *description;
  int item_id;
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
  return 1;
}

void ledger_ledger_clear(struct ledger_ledger* l){
  ledger_util_free(l->description);
  l->description = NULL;
  ledger_util_free(l->name);
  l->description = NULL;
  l->item_id = -1;
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
  return 1;
}


/* END   implementation */
