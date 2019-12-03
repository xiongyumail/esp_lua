COMPONENT_SRCS :=  esp_lua.c \
                   lua/lapi.c \
                   lua/lbaselib.c \
                   lua/lcode.c \
                   lua/lctype.c \
                   lua/ldebug.c \
                   lua/ldump.c \
                   lua/lgc.c \
                   lua/liolib.c \
                   lua/lmathlib.c \
                   lua/loadlib.c \
                   lua/lopcodes.c \
                   lua/lparser.c \
                   lua/lstring.c \
                   lua/ltable.c \
                   lua/ltm.c \
                   lua/lutf8lib.c \
                   lua/lzio.c \
                   lua/lauxlib.c \
                   lua/lbitlib.c \
                   lua/lcorolib.c \
                   lua/ldblib.c \
                   lua/ldo.c \
                   lua/lfunc.c \
                   lua/llex.c \
                   lua/lmem.c \
                   lua/lobject.c \
                   lua/loslib.c \
                   lua/lstate.c \
                   lua/lstrlib.c \
                   lua/ltablib.c \
                   lua/lundump.c \
                   lua/lvm.c \
                   lua/lua.c

COMPONENT_ADD_INCLUDEDIRS := lua \
                             include

CFLAGS += -DLUA_USE_IDF -DLUA_USE_C89 -DLUA_COMPAT_MATHLIB
CFLAGS += -Dmain=lua_main
