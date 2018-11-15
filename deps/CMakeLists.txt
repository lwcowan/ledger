
#dependencies

add_subdirectory(zip)
add_subdirectory(cJSON)

#minicsv library
option(MINICSV_BUILD_TESTS "Build test program for minicsv library")
add_library(minicsv "minicsv/minicsv.c" "minicsv/minicsv.h")
if (MINICSV_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)
  add_executable("minicsv-test" "minicsv/minicsv-test.c")
  target_link_libraries("minicsv-test" minicsv)
endif (MINICSV_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)

