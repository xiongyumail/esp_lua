#pragma once

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "esp_lua_port.h"

#ifdef __cplusplus
extern "C" {
#endif

void esp_lua_init(FILE *in, FILE *out, FILE *err, luaL_Reg *libs);

int esp_lua_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif