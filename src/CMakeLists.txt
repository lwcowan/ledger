


#set base library code
set(ledger_base_SOURCES
  base/util.h          base/util.c
  base/book.h          base/book.c
  base/ledger.h        base/ledger.c
  base/bignum.h        base/bignum.c
  base/account.h       base/account.c
  base/journal.h       base/journal.c
  base/entry.h         base/entry.c
  base/find.h          base/find.c
  base/table.h         base/table.c
  base/sum.h           base/sum.c
  )

add_library(ledger_base ${ledger_base_SOURCES})

#set io library code
set(ledger_io_SOURCES
  io/book.h            io/book.c
  io/manifest.h        io/manifest.c
  io/util.h            io/util.c
  io/ledger.h          io/ledger.c
  io/table.h           io/table.c
  io/account.h         io/account.c
  io/entry.h           io/entry.c
  io/journal.h         io/journal.c
  )

add_library(ledger_io ${ledger_io_SOURCES})
target_link_libraries(ledger_io  zip cjson minicsv)

#set action library code
set(ledger_act_SOURCES
  act/transact.h       act/transact.c
  act/path.h           act/path.c
  act/arg.h            act/arg.c
  act/commit.h         act/commit.c
  act/select.h         act/select.c
  )

add_library(ledger_act ${ledger_act_SOURCES})
target_link_libraries(ledger_act  ledger_base)

#set lua library code
set(ledger_lua_SOURCES
  lua/luaext.h         lua/luaext.c
  )

add_library(ledger_lua ${ledger_lua_SOURCES})
target_link_libraries(ledger_lua ledger_act ledger_base lua)

add_executable(ledger_lua_exec lua/lua.c)
target_link_libraries(ledger_lua_exec ledger_lua linenoise)

#set ledger-cli code
set(ledger_cli_SOURCES
  cli/line.h           cli/line.c
  cli/test.h           cli/test.c
  cli/quit.h           cli/quit.c
  cli/iocmd.h          cli/iocmd.c
  cli/navigate.h       cli/navigate.c
  cli/manage.h         cli/manage.c
  cli/rename.h         cli/rename.c
  cli/select.h         cli/select.c
  cli/print.h          cli/print.c
  cli/cli.c
  )

add_executable(ledger_cli ${ledger_cli_SOURCES})
target_link_libraries(ledger_cli
  ledger_act ledger_io ledger_base linenoise)
