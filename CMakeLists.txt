set(COMPONENT_SRCS esp_lua.c
                   src/lapi.c
                   src/lbaselib.c
                   src/lcode.c
                   src/lctype.c
                   src/ldebug.c
                   src/ldump.c
                   src/lgc.c
                   src/liolib.c
                   src/lmathlib.c
                   src/loadlib.c
                   src/lopcodes.c
                   src/lparser.c
                   src/lstring.c
                   src/ltable.c
                   src/ltm.c
                   src/lutf8lib.c
                   src/lzio.c
                   src/lauxlib.c
                   src/lbitlib.c
                   src/lcorolib.c
                   src/ldblib.c
                   src/ldo.c
                   src/lfunc.c
                   src/linit.c
                   src/llex.c
                   src/lmem.c
                   src/lobject.c
                   src/loslib.c
                   src/lstate.c
                   src/lstrlib.c
                   src/ltablib.c
                   src/lundump.c
                   src/lvm.c
                   src/lua.c)

set(COMPONENT_ADD_INCLUDEDIRS src include)

register_component()

target_compile_definitions(${COMPONENT_LIB} PRIVATE -DLUA_USE_IDF -DLUA_USE_C89 -DLUA_COMPAT_MATHLIB)
target_compile_definitions(${COMPONENT_LIB} PRIVATE -Dmain=lua_main)