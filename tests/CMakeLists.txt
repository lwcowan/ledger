

#utility test
add_executable("ledger_test_util" "test_util.c")

target_link_libraries("ledger_test_util" ledger_base)