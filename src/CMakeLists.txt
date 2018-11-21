


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
