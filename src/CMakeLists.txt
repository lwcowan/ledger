


#set base library code
set(ledger_base_SOURCES
  base/util.h          base/util.c
  base/book.h          base/book.c
  )

add_library(ledger_base ${ledger_base_SOURCES})

#set io library code
set(ledger_io_SOURCES
  io/book.h            io/book.c
  io/manifest.h        io/manifest.c
  )

add_library(ledger_io ${ledger_io_SOURCES})
