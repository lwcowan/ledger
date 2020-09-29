
#dependencies

add_subdirectory(cJSON)
add_subdirectory(zip)
add_subdirectory(lua)
add_subdirectory(refalloc)

#minicsv library
option(MINICSV_BUILD_TESTS "Build test program for minicsv library")
add_library(minicsv "minicsv/minicsv.c" "minicsv/minicsv.h")
if (MINICSV_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)
  add_executable("minicsv-test" "minicsv/minicsv-test.c")
  target_link_libraries("minicsv-test" minicsv)
endif (MINICSV_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)

#linenoise library
option(LINENOISE_BUILD_TESTS "Build test program for linenoise library")
option(LINENOISE_USE_UTF8 "Build UTF-8 support for linenoise library")
set(LINENOISE_boat_files
  "linenoise/linenoise.c"
  "linenoise/linenoise.h"
  "linenoise/stringbuf.c"
  "linenoise/stringbuf.h"
  )
if (LINENOISE_USE_UTF8)
  list(APPEND LINENOISE_boat_files
    "linenoise/utf8.c"
    "linenoise/utf8.h"
    )
endif(LINENOISE_USE_UTF8)
#if(WIN32)
#  #this file included automatically by ".../linenoise.c"
#  list(APPEND LINENOISE_boat_files
#    "linenoise/linenoise-win32.c"
#    )
#endif(WIN32)
add_library(linenoise STATIC ${LINENOISE_boat_files})
if (LINENOISE_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)
  add_executable("linenoise-example" "linenoise/example.c")
  target_link_libraries("linenoise-example" linenoise)
  add_executable("linenoise-teststringbuf" "linenoise/teststringbuf.c")
  target_link_libraries("linenoise-teststringbuf" linenoise)
endif (LINENOISE_BUILD_TESTS AND NOT CMAKE_DISABLE_TESTING)

