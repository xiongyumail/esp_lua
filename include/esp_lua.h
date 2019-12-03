#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void esp_lua_init(FILE *in, FILE *out, FILE *err, const luaL_Reg *libs);

int esp_lua_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif