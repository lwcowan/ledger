
#include "arg.h"
#include "../base/util.h"
#include <limits.h>


struct ledger_arg_list {
  int argc;
  char** argv;
};

/*
 * Initialize a list.
 * - l list to initialize
 * @return one on success, zero on failure
 */
static int ledger_arg_list_init(struct ledger_arg_list* l);

/*
 * Clear out a list.
 * - l list to clear
 */
static void ledger_arg_list_clear(struct ledger_arg_list* l);

/*
 * Flatten a quoted string in place.
 * - s string to flatten
 */
static void ledger_arg_flatten(char* s);

/* BEGIN static implementation */

int ledger_arg_list_init(struct ledger_arg_list* l){
  l->argv = NULL;
  l->argc = 0;
  return 1;
}

void ledger_arg_list_clear(struct ledger_arg_list* l){
  int i;
  for (i = 0; i < l->argc; ++i){
    ledger_util_free(l->argv[i]);
  }
  ledger_util_free(l->argv);
  l->argv = NULL;
  l->argc = 0;
  return;
}

/* END   static implementation */

/* BEGIN implementation */

void ledger_arg_flatten(char* s){
  int quote_enter = 0;
  char *t, *back_point = s;
  for (t = s; *t; ++t){
    if (*t == '"'){
      switch (quote_enter){
      case -1:
        /* put a single double quote */
        *back_point = '"';
        back_point += 1;
      case 0:
        /* no quote yet */
        quote_enter = 1;
        break;
      case +1:
        quote_enter = -1;
        break;
      }
    } else {
      if (quote_enter == -1)
        quote_enter = 0;
      *back_point = *t;
      back_point += 1;
    }
  }
  *back_point = 0;
  return;
}

struct ledger_arg_list* ledger_arg_list_new(void){
  struct ledger_arg_list* n = (struct ledger_arg_list* )ledger_util_malloc
    (sizeof(struct ledger_arg_list));
  if (n != NULL){
    if (!ledger_arg_list_init(n)){
      ledger_util_free(n);
      n = NULL;
    }
  }
  return n;
}

void ledger_arg_list_free(struct ledger_arg_list* n){
  if (n != NULL){
    ledger_arg_list_clear(n);
    ledger_util_free(n);
  }
}

int ledger_arg_list_is_equal
  (struct ledger_arg_list const* a, struct ledger_arg_list const* b)
{
  /* trivial journals */
  if (a == NULL && b == NULL) return 1;
  else if (a == NULL || b == NULL) return 0;
  /* compare argv */{
    int i;
    if (a->argc != b->argc) return 0;
    else for (i = 0; i < a->argc; ++i){
      if (ledger_util_ustrcmp(
          (unsigned char const*)a->argv[i],
          (unsigned char const*)b->argv[i]) != 0)
        break;
    }
    if (i < a->argc) return 0;
  }
  return 1;
}

int ledger_arg_list_get_count(struct ledger_arg_list const* a){
  return a->argc;
}

char const* ledger_arg_list_get(struct ledger_arg_list const* a, int i){
  if (i < 0 || i >= a->argc){
    return NULL;
  } else {
    return a->argv[i];
  }
}

int ledger_arg_list_set_count(struct ledger_arg_list* a, int n){
  if (n >= INT_MAX/sizeof(char*)){
    return 0;
  } else if (n < 0){
    return 0;
  } else if (n == 0){
    ledger_arg_list_clear(a);
    return 1;
  } else if (n < a->argc){
    int i;
    /* allocate smaller array */
    char** new_array = (char** )ledger_util_malloc(n*sizeof(char*));
    if (new_array == NULL) return 0;
    /* move old argv to new array */
    for (i = 0; i < n; ++i){
      new_array[i] = a->argv[i];
    }
    /* free rest of the argv */
    for (; i < a->argc; ++i){
      ledger_util_free(a->argv[i]);
    }
    ledger_util_free(a->argv);
    a->argv = new_array;
    a->argc = n;
    return 1;
  } else if (n >= a->argc){
    int save_id;
    int i;
    /* allocate larger array */
    char** new_array = (char** )ledger_util_malloc(n*sizeof(char*));
    if (new_array == NULL) return 0;
    /* make new argv */
    for (i = a->argc; i < n; ++i){
      new_array[i] = NULL;
    }
    /* transfer old argv */
    for (i = 0; i < a->argc; ++i){
      new_array[i] = a->argv[i];
    }
    /* continue */
    ledger_util_free(a->argv);
    a->argv = new_array;
    a->argc = n;
    return 1;
  } else return 1 /*since n == a->argc */;
}

int ledger_arg_list_set(struct ledger_arg_list const* a, int i, char const* s){
  if (i < 0 || i >= a->argc){
    return 0;
  } else {
    int ok;
    char* new_string = (char*)ledger_util_ustrdup(
        (unsigned char const*)s, &ok);
    if (!ok) return 0;
    else {
      ledger_util_free(a->argv[i]);
      a->argv[i] = new_string;
      return 1;
    }
  }
}

int ledger_arg_list_parse(struct ledger_arg_list* a, char const* command){
  int token_count = 0;
  char** total_pieces = NULL;
  /* first pass: count the tokens */{
    char const* p;
    int quote = 0;
    int alpha_count = 0;
    for (p = command; *p; ++p){
      if (quote){
        if (*p == '"')
          quote = 0;
          alpha_count += 1;
      } else {
        if (*p == '"'){
          alpha_count += 1;
          quote = 1;
        } else if (*p == ' ') {
          if (alpha_count > 0){
            token_count += 1;
            alpha_count = 0;
          }
        } else {
          alpha_count += 1;
        }
      }
    }
    if (alpha_count > 0){
      token_count += 1;
    }
    total_pieces = ledger_util_malloc(sizeof(char*)*(token_count+1));
    if (total_pieces == NULL){
      return 0;
    } else total_pieces[token_count] = NULL;
  }
  /* second pass: extract the tokens */{
    char const* p;
    char const* token_start = command;
    int quote = 0;
    int ok = 1;
    int micro_token_count = 0;
    int alpha_count = 0;
    for (p = command; *p; ++p){
      if (quote){
        if (*p == '"') {
          quote = 0;
          alpha_count += 1;
        } else {
          alpha_count += 1;
        }
      } else {
        if (*p == '"'){
          alpha_count += 1;
          quote = 1;
        } else if (*p == ' ') {
          if (alpha_count > 0){
            total_pieces[micro_token_count] =
              ledger_util_ustrndup(token_start, alpha_count, &ok);
            if (!ok) break;
            micro_token_count += 1;
            alpha_count = 0;
          }
          token_start = p+1;
          if (micro_token_count >= token_count) break;
        } else {
          alpha_count += 1;
        }
      }
    }
    if (ok && alpha_count > 0 && micro_token_count < token_count){
      total_pieces[micro_token_count] =
        ledger_util_ustrndup(token_start, alpha_count, &ok);
      if (ok) micro_token_count += 1;
    }
    if (!ok){
      int i;
      for (i = 0; i < micro_token_count; ++i){
        ledger_util_free(total_pieces[i]);
      }
      return 0;
    } else token_count = micro_token_count;
  }
  /* third pass: flatten strings */{
    int i;
    for (i = 0; i < token_count; ++i){
      ledger_arg_flatten(total_pieces[i]);
    }
  }
  /* clear and replace */
  ledger_arg_list_clear(a);
  a->argv = total_pieces;
  a->argc = token_count+1;
  return 1;
}

char const** ledger_arg_list_fetch(struct ledger_arg_list const* a){
  return (char const**)a->argv;
}


/* END   implementation */

