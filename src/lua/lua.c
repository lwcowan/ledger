
#include <stdio.h>
#include "luaext.h"
#include "../../deps/linenoise/linenoise.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
  int result = 0;
  struct ledger_lua* lua;
  int fname_point;
  int i_mode = 0;
  int f_mode = 0;
  for (fname_point = 1; fname_point < argc; ++fname_point){
    if (strcmp("-i",argv[fname_point]) == 0){
      i_mode = 1;
    } else if (strcmp("--",argv[fname_point]) == 0){
      fname_point += 1;
      break;
    } else break;
  }
  if (fname_point >= argc){
    /* assert interactive mode */
    fname_point = 0;
    i_mode = 1;
  } else {
    f_mode = 1;
  }
  linenoiseHistorySetMaxLen(50);
  lua = ledger_lua_new();
  if (lua != NULL) do {
    if (!ledger_lua_openlibs(lua)){
      fputs("ledger_lua: standard library initialization "
          "did not succeed\n",stderr);
      result = 0;
      break;
    }
    if (!ledger_lua_set_arg(lua, argv, argc, fname_point)){
        fputs("ledger_lua: argument configuration did not succeed\n",stderr);
        result = 0;
      break;
    }
    if (f_mode){
      result = ledger_lua_exec_file
        ( lua, (unsigned char const*)argv[fname_point], argv[fname_point]);
      if (!result){
        unsigned char const* errmsg = ledger_lua_get_last_error(lua);
        if (errmsg != NULL){
          fputs("ledger_lua: script chunk rejected: {\n",stderr);
          fputs((char const*)errmsg,stderr);
          fputs("\n}\n",stderr);
        } else fputs("ledger_lua: script chunk rejected\n",stderr);
        result = 0;
        break;
      }
    }
    if (i_mode){
      /* do interactive mode */
      char* next_line;
      int ok;
      int want_continue = 0;
      result = 1;
      while ((next_line = linenoise(
          want_continue ? "        + > " : "ledger_lua> "
        )) != NULL)
      {
        linenoiseHistoryAdd(next_line);
        ok = ledger_lua_exec_str
          ( lua, (unsigned char const*)"*", (unsigned char const*)next_line,
            want_continue, &want_continue);
        free(next_line);
        if (!ok){
          unsigned char const* errmsg = ledger_lua_get_last_error(lua);
          if (errmsg != NULL){
            fputs("ledger_lua: script line rejected: {\n",stderr);
            fputs((char const*)errmsg,stderr);
            fputs("\n}\n",stderr);
          } else fputs("ledger_lua: script line rejected\n",stderr);
          result = 0;
        }
      }
    }
    result = 1;
  } while (0); else result = 0;
  ledger_lua_close(lua);
  return result?EXIT_SUCCESS:EXIT_FAILURE;
}
