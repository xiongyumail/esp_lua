#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef size_t (*esp_lua_output_callback_t)(char* str, size_t len);

size_t esp_lua_input(char* str, size_t len);

void esp_lua_init(esp_lua_output_callback_t output_cb, const luaL_Reg *libs, const char *history);

int esp_lua_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif