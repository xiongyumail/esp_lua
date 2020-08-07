COMPONENT_OBJS :=  lua/lapi.o \
                   lua/lbaselib.o \
                   lua/lcode.o \
                   lua/lctype.o \
                   lua/ldebug.o \
                   lua/ldump.o \
                   lua/lgc.o \
                   lua/liolib.o \
                   lua/lmathlib.o \
                   lua/loadlib.o \
                   lua/lopcodes.o \
                   lua/lparser.o \
                   lua/lstring.o \
                   lua/ltable.o \
                   lua/ltm.o \
                   lua/lutf8lib.o \
                   lua/lzio.o \
                   lua/lauxlib.o \
                   lua/lbitlib.o \
                   lua/lcorolib.o \
                   lua/ldblib.o \
                   lua/ldo.o \
                   lua/lfunc.o \
                   lua/llex.o \
                   lua/lmem.o \
                   lua/lobject.o \
                   lua/loslib.o \
                   lua/lstate.o \
                   lua/lstrlib.o \
                   lua/ltablib.o \
                   lua/lundump.o \
                   lua/lvm.o \
                   lua/lua.o \
                   lua_linenoise/lua_linenoise.o \
                   esp_lua.o

COMPONENT_SRCDIRS := lua lua_linenoise .

COMPONENT_ADD_INCLUDEDIRS := lua include lua_linenoise

CFLAGS += -DLUA_USE_IDF -DLUA_USE_C89 -DLUA_COMPAT_MATHLIB 
CFLAGS += -Dmain=lua_main
