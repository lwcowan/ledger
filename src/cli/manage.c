
#include "manage.h"
#include "../base/book.h"
#include "../base/journal.h"
#include "../base/ledger.h"
#include "../base/account.h"
#include "../base/entry.h"
#include "../base/table.h"
#include "../base/find.h"
#include "line.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int ledger_cli_make_ledger
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* id_string = NULL;
  int id_note = -1;
  int ledger_index;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-i") == 0){
      if (++argi < argc){
        id_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_ledger: Make a ledger.\n"
      "usage: make_ledger [options] (name)\n"
      "options:\n"
      "  (name)           set name for new ledger\n"
      "  -d (string)      set description for new ledger\n"
      "  -i (number)      id number for new ledger\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* sanity check on names and identifiers */{
    int corresponding_index;
    corresponding_index =
      ledger_find_ledger_by_name(tracking->book, name_string);
    if (corresponding_index >= 0){
      fputs("make_ledger: Ledger by name already exists\n", stderr);
      return 1;
    }
    if (id_string != NULL){
      id_note = atoi(id_string);
      if (id_note < 0){
        fputs("make_ledger: Negative ID not allowed\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_ledger_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_ledger: Ledger by ID already exists\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_journal_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_ledger: Journal by ID already exists\n", stderr);
        return 1;
      }
    }
  }
  /* make the ledger */{
    int ok;
    ledger_index = ledger_book_get_ledger_count(tracking->book);
    ok = ledger_book_set_ledger_count(tracking->book, ledger_index+1);
    if (!ok){
      fputs("make_ledger: Error when making ledger structure\n", stderr);
      return 1;
    } else {
      int soft_ok;
      struct ledger_ledger* ledger =
        ledger_book_get_ledger(tracking->book, ledger_index);
      if (id_string != NULL){
        ledger_ledger_set_id(ledger, id_note);
      }
      soft_ok = ledger_ledger_set_name(ledger, name_string);
      if (!soft_ok){
        fputs("make_ledger: warning: Unable to set name\n", stderr);
      }
      soft_ok = ledger_ledger_set_description(ledger, description_string);
      if (!soft_ok){
        fputs("make_ledger: warning: Unable to set description\n", stderr);
      }
    }
  }
  fprintf(stderr, "make_ledger: Created ledger@%i\n", ledger_index);
  return 0;
}


int ledger_cli_make_journal
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* id_string = NULL;
  int id_note = -1;
  int journal_index;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-i") == 0){
      if (++argi < argc){
        id_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_journal: Make a journal.\n"
      "usage: make_journal [options] (name)\n"
      "options:\n"
      "  (name)           set name for new journal\n"
      "  -d (string)      set description for new journal\n"
      "  -i (number)      id number for new journal\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* sanity check on names and identifiers */{
    int corresponding_index;
    corresponding_index =
      ledger_find_journal_by_name(tracking->book, name_string);
    if (corresponding_index >= 0){
      fputs("make_journal: Journal by name already exists\n", stderr);
      return 1;
    }
    if (id_string != NULL){
      id_note = atoi(id_string);
      if (id_note < 0){
        fputs("make_journal: Negative ID not allowed\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_journal_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_journal: Journal by ID already exists\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_ledger_by_id(tracking->book, id_note);
      if (corresponding_index >= 0){
        fputs("make_journal: Ledger by ID already exists\n", stderr);
        return 1;
      }
    }
  }
  /* make the journal */{
    int ok;
    journal_index = ledger_book_get_journal_count(tracking->book);
    ok = ledger_book_set_journal_count(tracking->book, journal_index+1);
    if (!ok){
      fputs("make_journal: Error when making journal structure\n", stderr);
      return 1;
    } else {
      int soft_ok;
      struct ledger_journal* journal =
        ledger_book_get_journal(tracking->book, journal_index);
      if (id_string != NULL){
        ledger_journal_set_id(journal, id_note);
      }
      soft_ok = ledger_journal_set_name(journal, name_string);
      if (!soft_ok){
        fputs("make_journal: warning: Unable to set name\n", stderr);
      }
      soft_ok = ledger_journal_set_description(journal, description_string);
      if (!soft_ok){
        fputs("make_journal: warning: Unable to set description\n", stderr);
      }
    }
  }
  fprintf(stderr, "make_journal: Created journal@%i\n", journal_index);
  return 0;
}



int ledger_cli_make_account
  (struct ledger_cli_line *tracking, int argc, char **argv)
{
  int argi;
  int help_requested = 0;
  char const* name_string = NULL;
  char const* description_string = NULL;
  char const* id_string = NULL;
  char const* pick_ledger_string = NULL;
  int id_note = -1;
  int account_index;
  int ledger_index;
  struct ledger_ledger *pick_ledger;
  if (tracking->object_path.len >= 1
  &&  tracking->object_path.typ == LEDGER_ACT_PATH_LEDGER)
  {
    ledger_index = tracking->object_path.path[0];
  } else ledger_index = -1;
  /* scan arguments */
  for (argi = 1; argi < argc; ++argi){
    if (strcmp(argv[argi],"-d") == 0){
      if (++argi < argc){
        description_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-i") == 0){
      if (++argi < argc){
        id_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-l") == 0){
      if (++argi < argc){
        pick_ledger_string = argv[argi];
      }
    } else if (strcmp(argv[argi],"-?") == 0){
      help_requested = 1;
    } else {
      name_string = argv[argi];
    }
  }
  if (help_requested || name_string == NULL){
    fputs("make_account: Make an account.\n"
      "usage: make_account [options] (name)\n"
      "options:\n"
      "  (name)           set name for new account\n"
      "  -d (string)      set description for new account\n"
      "  -i (number)      id number for new account\n"
      "  -l (path)        path to desired container ledger\n"
      "  -?               help text\n"
      ,stderr);
    return 2;
  }
  /* resolve ledger path */if (pick_ledger_string != NULL){
    int ok;
    struct ledger_act_path new_path =
      ledger_act_path_compute(tracking->book, pick_ledger_string,
      tracking->object_path, &ok);
    if (!ok){
      fputs("make_account: Error processing path to ledger\n", stderr);
      return 1;
    } else if (new_path.typ != LEDGER_ACT_PATH_LEDGER){
      fputs("make_account: Path does not point to a ledger\n", stderr);
      return 1;
    } else {
      ledger_index = new_path.path[0];
    }
  }
  if (ledger_index < 0){
    fputs("make_account: Choose a ledger before making an account\n", stderr);
    return 1;
  }
  pick_ledger = ledger_book_get_ledger(tracking->book, ledger_index);
  if (pick_ledger == NULL){
    fputs("make_account: Chosen ledger is unavailable\n", stderr);
    return 1;
  }
  /* sanity check on names and identifiers */{
    int corresponding_index;
    corresponding_index =
      ledger_find_account_by_name(pick_ledger, name_string);
    if (corresponding_index >= 0){
      fputs("make_account: Journal by name already exists\n", stderr);
      return 1;
    }
    if (id_string != NULL){
      id_note = atoi(id_string);
      if (id_note < 0){
        fputs("make_account: Negative ID not allowed\n", stderr);
        return 1;
      }
      corresponding_index =
        ledger_find_account_by_id(pick_ledger, id_note);
      if (corresponding_index >= 0){
        fputs("make_account: Journal by ID already exists\n", stderr);
        return 1;
      }
    }
  }
  /* make the account */{
    int ok;
    account_index = ledger_ledger_get_account_count(pick_ledger);
    ok = ledger_ledger_set_account_count(pick_ledger, account_index+1);
    if (!ok){
      fputs("make_account: Error when making account structure\n", stderr);
      return 1;
    } else {
      int soft_ok;
      struct ledger_account* account =
        ledger_ledger_get_account(pick_ledger, account_index);
      if (id_string != NULL){
        ledger_account_set_id(account, id_note);
      }
      soft_ok = ledger_account_set_name(account, name_string);
      if (!soft_ok){
        fputs("make_account: warning: Unable to set name\n", stderr);
      }
      soft_ok = ledger_account_set_description(account, description_string);
      if (!soft_ok){
        fputs("make_account: warning: Unable to set description\n", stderr);
      }
    }
  }
  fprintf(stderr, "make_account: Created account@%i\n", account_index);
  return 0;
}
