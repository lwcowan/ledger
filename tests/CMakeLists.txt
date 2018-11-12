

#utility test
add_executable("ledger_test_util" "test_util.c")
#book test
add_executable("ledger_test_book" "test_book.c")
#io_book test
add_executable("ledger_test_io_book" "test_io_book.c")
#manifest test
add_executable("ledger_test_io_manifest" "test_io_manifest.c")

target_link_libraries("ledger_test_util" ledger_base)
target_link_libraries("ledger_test_book" ledger_base)
target_link_libraries("ledger_test_io_book" ledger_io ledger_base)
target_link_libraries("ledger_test_io_manifest" ledger_io ledger_base cjson)
