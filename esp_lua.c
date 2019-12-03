#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linenoise.h"
#include "ring_buf.h"
#include "esp_lua.h"

static char *history_filename = NULL;
static ring_buf_t ring_buf_input;
static luaL_Reg *esp_lua_libs = NULL;
static esp_lua_output_callback_t esp_lua_output_cb = NULL;

static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_UTF8LIBNAME, luaopen_utf8},
  {LUA_DBLIBNAME, luaopen_debug},
#if defined(LUA_COMPAT_BITLIB)
  {LUA_BITLIBNAME, luaopen_bit32},
#endif
  {NULL, NULL}
};

void esp_luaL_openlibs(lua_State *L) 
{
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }

  if (esp_lua_libs != NULL) {
    for (lib = esp_lua_libs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }
  }
}

size_t esp_lua_read(char *ptr, size_t len)
{
    size_t x = 0;
    if (ptr == NULL) {
        return 0;
    }
    for (x = 0; x < len;) {
        if (ring_buf_get(&ring_buf_input, (uint8_t *)&ptr[x], 1) == -1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        } else {
            x++;
        }
    }
    
    return x;
}

size_t esp_lua_write(char *ptr, size_t len)
{
    if (ptr == NULL || esp_lua_output_cb == NULL) {
        return 0;
    }

    return esp_lua_output_cb(ptr, len);
}

int esp_lua_printf(const char *fmt, ...)
{
    int ret = 0;
    char *str = calloc(LUA_MAXINPUT, sizeof(char));
    va_list arg_list;
    va_start(arg_list, fmt);
    vsnprintf(str, LUA_MAXINPUT, fmt, arg_list);
    va_end(arg_list);
    ret = esp_lua_write((void *)str, strlen(str));
    free(str);

    return ret;
}

int esp_lua_putc(int c)
{
    return esp_lua_write((void *)&c, 1);
}

int esp_lua_system(const char * string)
{
    return 0;
}

void (*esp_lua_signal(int sig, void (*func)(int)))(int)
{
    return 0;
}

void esp_lua_exit(int status)
{
    // exit_flag = 1;
}

size_t esp_lua_writestring(const char *str, size_t size)
{
    return esp_lua_write((const void *)str, size);
}

void esp_lua_writeline(void)
{
    esp_lua_writestring("\n", 1);
}

void esp_lua_writestringerror(const char *fmt, ...)
{
    char *str = calloc(LUA_MAXINPUT, sizeof(char));
    va_list arg_list;
    va_start(arg_list, fmt);
    vsnprintf(str, LUA_MAXINPUT, fmt, arg_list); // I don't know why vfprintf(fout...) can't use when fout != stderr.
    va_end(arg_list);
    // esp_lua_printf("\n\n");
    esp_lua_printf(str);
    free(str);
}

void esp_lua_completion_callback(const char *buf, linenoiseCompletions *lc) {
    if (!strcasecmp(buf,"mylib")) {
        linenoiseAddCompletion(lc,"mylib.hello()");
    }
}

char *esp_lua_hints_callback(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"mylib")) {
        *color = 32;
        *bold = 0;
        return ".hello()";
    }
    return NULL;
}

size_t esp_lua_input(char* str, size_t len)
{
    size_t x = 0;
    if (ring_buf_input.p_o == NULL) {
        return 0;
    }
    for (x = 0; x < len;) {
        if (ring_buf_put(&ring_buf_input, str[x]) == -1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        } else {
            x++;
        }
    }
    
    return x;
}

void esp_lua_init(esp_lua_output_callback_t output_cb, const luaL_Reg *libs, const char *history)
{
    char *buffer_in = calloc(sizeof(char), LUA_MAXINPUT);
    ring_buf_init(&ring_buf_input, (uint8_t *)buffer_in, LUA_MAXINPUT);
    esp_lua_output_cb = output_cb;
    history_filename = history;
    esp_lua_libs = (luaL_Reg *)libs;
    linenoiseSetMultiLine(1);
    // linenoiseSetDumbMode(1);
    // linenoisePrintKeyCodes();
    linenoiseSetCompletionCallback(esp_lua_completion_callback);
    linenoiseSetHintsCallback(esp_lua_hints_callback);
}

int esp_lua_main(int argc, char **argv)
{
    int ret = 0;
    extern int lua_main (int argc, char **argv);

    linenoiseClearScreen();
    if (history_filename) {
        linenoiseHistoryLoad(history_filename);
    }
    ret = lua_main (argc, argv);
    if (history_filename) {
        linenoiseHistorySave(history_filename);
    }
    
    return ret;
}