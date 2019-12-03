#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#if !defined(ESP_LUA_HISTORY_PATH)
#define ESP_LUA_HISTORY_PATH "/lua/history.txt"
#endif

typedef size_t (*esp_lua_callback_t)(char* str, size_t len);

void esp_lua_init(esp_lua_callback_t input_cb, esp_lua_callback_t output_cb, const luaL_Reg *libs);

int esp_lua_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif