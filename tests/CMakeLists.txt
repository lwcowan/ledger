

#utility test
add_executable("ledger_test_util" "test_util.c")
#book test
add_executable("ledger_test_book" "test_book.c")
#ledger test
add_executable("ledger_test_ledger" "test_ledger.c")
#big number test
add_executable("ledger_test_bignum" "test_bignum.c")
#table test
add_executable("ledger_test_table" "test_table.c")
#journal test
add_executable("ledger_test_journal" "test_journal.c")
#big number test
add_executable("ledger_test_account" "test_account.c")

target_link_libraries("ledger_test_util" ledger_base)
target_link_libraries("ledger_test_book" ledger_base)
target_link_libraries("ledger_test_ledger" ledger_base)
target_link_libraries("ledger_test_bignum" ledger_base)
target_link_libraries("ledger_test_table" ledger_base)
target_link_libraries("ledger_test_journal" ledger_base)
target_link_libraries("ledger_test_account" ledger_base)
