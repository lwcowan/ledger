
cmake_minimum_required(VERSION 3.0)

project(ledger C)

option(LEDGER_BUILD_TESTS "Build tests for the ledger library.")
option(CMAKE_DISABLE_TESTING "Disable testing.")

add_subdirectory(deps)

add_subdirectory(src)

if (LEDGER_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)
  add_subdirectory(tests)
endif(LEDGER_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)
