
cmake_minimum_required(VERSION 3.0)


option(LUA_32BITS "LuaConf: enables Lua with 32-bit integers and 32-bit floats" OFF)
option(LUA_USE_C89 "LuaConf: controls the use of non-ISO-C89 features" OFF)
set(LUAI_MAXCSTACK CACHE STRING "LuaConf: maximum depth for nested calls and other recursive algorithms")
set(LUA_PATH_DEFAULT CACHE PATH "LuaConf: default path that Lua uses to look for Lua libraries")
set(LUA_CPATH_DEFAULT CACHE PATH "LuaConf: default path that Lua uses to look for C libraries")
set(LUA_DIRSEP CACHE STRING "LuaConf: directory separator (for submodules)")

option(BUILD_SHARED_LIBS "Build shared libraries")

if (BUILD_SHARED_LIBS)
  if (WIN32)
    set(LUA_BUILD_AS_DLL ON)
  endif(WIN32)
endif(BUILD_SHARED_LIBS)

set(lua_CORE_FILES
    src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c src/ldump.c
    src/lfunc.c src/lgc.c src/llex.c src/lmem.c src/lobject.c src/lopcodes.c
    src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c
    src/lundump.c src/lvm.c src/lzio.c

    src/lapi.h src/lcode.h src/lctype.h src/ldebug.h src/ldo.h
    src/lfunc.h src/lgc.h src/llex.h src/lmem.h src/lobject.h src/lopcodes.h
    src/lparser.h src/lstate.h src/lstring.h src/ltable.h src/ltm.h
    src/lundump.h src/lvm.h src/lzio.h

    src/llimits.h src/lprefix.h src/lua.h src/luaconf.h

    src/ljumptab.h src/lopnames.h
  )
set(lua_LIB_FILES
    src/lauxlib.c src/lbaselib.c src/lcorolib.c src/ldblib.c
    src/liolib.c src/lmathlib.c src/loslib.c src/lstrlib.c src/ltablib.c
    src/lutf8lib.c src/loadlib.c src/linit.c

    src/lauxlib.h

    src/lualib.h
  )

add_library(lua ${lua_CORE_FILES} ${lua_LIB_FILES})

#BEGIN  configure
if (LUAI_MAXCSTACK GREATER 0)
  target_compile_definitions(lua PRIVATE "LUAI_MAXCSTACK=${LUAI_MAXCSTACK}")
endif (LUAI_MAXCSTACK GREATER 0)
if (LUA_USE_C89)
  target_compile_definitions(lua PUBLIC "LUA_USE_C89")
endif (LUA_USE_C89)
if (LUA_32BITS)
  target_compile_definitions(lua PUBLIC "LUA_32BITS")
endif (LUA_32BITS)
if (LUA_PATH_DEFAULT)
  target_compile_definitions(lua PRIVATE "LUA_PATH_DEFAULT=\"${LUA_PATH_DEFAULT}\"")
endif (LUA_PATH_DEFAULT)
if (LUA_CPATH_DEFAULT)
  target_compile_definitions(lua PRIVATE "LUA_CPATH_DEFAULT=\"${LUA_CPATH_DEFAULT}\"")
endif (LUA_CPATH_DEFAULT)
if (LUA_DIRSEP)
  target_compile_definitions(lua PRIVATE "LUA_DIRSEP=\"${LUA_DIRSEP}\"")
endif (LUA_DIRSEP)
#END    configure

if (UNIX)
  find_library(m_LIBRARY NAMES m)
  target_link_libraries(lua ${m_LIBRARY})
  target_compile_definitions(lua PRIVATE "LUA_USE_POSIX")
endif(UNIX)
