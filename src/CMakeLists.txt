


#set base library code
set(ledger_base_SOURCES
  base/util.h          base/util.c
  base/book.h          base/book.c
  base/ledger.h        base/ledger.c
  base/bignum.h        base/bignum.c
  base/account.h       base/account.c
  base/journal.h       base/journal.c
  base/entry.h         base/entry.c
  base/table.h         base/table.c
  )

add_library(ledger_base ${ledger_base_SOURCES})
