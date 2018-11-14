

#utility test
add_executable("ledger_test_util" "test_util.c")
#book test
add_executable("ledger_test_book" "test_book.c")
#ledger test
add_executable("ledger_test_ledger" "test_ledger.c")
#big number test
add_executable("ledger_test_bignum" "test_bignum.c")

target_link_libraries("ledger_test_util" ledger_base)
target_link_libraries("ledger_test_book" ledger_base)
target_link_libraries("ledger_test_ledger" ledger_base)
target_link_libraries("ledger_test_bignum" ledger_base)


#io_util test
add_executable("ledger_test_io_util" "test_io_util.c")
#io_book test
add_executable("ledger_test_io_book" "test_io_book.c")
#manifest test
add_executable("ledger_test_io_manifest" "test_io_manifest.c")

target_link_libraries("ledger_test_io_book" ledger_io ledger_base)
target_link_libraries("ledger_test_io_manifest" ledger_io ledger_base cjson)
target_link_libraries("ledger_test_io_util" ledger_io ledger_base cjson)
