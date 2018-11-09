
cmake_minimum_required(VERSION 3.0)

project(ledger C)

option(LEDGER_BUILD_TESTS "Build tests for the ledger library.")

add_subdirectory(deps)

add_subdirectory(src)

if (LEDGER_BUILD_TESTS)
  add_subdirectory(tests)
endif(LEDGER_BUILD_TESTS)
