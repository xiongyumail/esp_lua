#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lua.h"

static int exit_flag = 0;
static luaL_Reg *esp_lua_libs = NULL;
static esp_lua_callback_t esp_lua_input_cb = NULL;
static esp_lua_callback_t esp_lua_output_cb = NULL;

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

static size_t esp_lua_input_callback_default(char *str, size_t len) 
{
    return fread(str, sizeof(char), len, stdin);
}

static size_t esp_lua_output_callback_default(char *str, size_t len) 
{
    return fwrite(str, sizeof(char), len, stdout);
}

size_t esp_lua_read(char *ptr, size_t len)
{
    size_t ret = 0;
    size_t x = 0;

    if (ptr == NULL) {
        return 0;
    }
    
    char *str = (char *)calloc(1, len + 1);
    for (x = 0; x < len;) {
        if (exit_flag) { /* Ctrl-c */
            free(str);
            ptr[0] = 3;
            exit_flag = 0;
            return 1;
        } else if ((ret = esp_lua_input_cb(str, len - x)) == 0) {
            vTaskDelay(10 / portTICK_RATE_MS);
        } else {
            memcpy(&ptr[x], str, ret);
            x += ret;
        }
    }
    free(str);
    ptr[x] = '\0';
    
    return x;
}

size_t esp_lua_write(char *ptr, size_t len)
{
    if (ptr == NULL) {
        return 0;
    }

    return esp_lua_output_cb(ptr, len);
}

int esp_lua_printf(const char *fmt, ...)
{
    int ret = 0;
    char *str = (char *)calloc(1, LUA_MAXINPUT + 1);
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
    exit_flag = 1;
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
    char *str = (char *)calloc(1, LUA_MAXINPUT + 1);
    va_list arg_list;
    va_start(arg_list, fmt);
    vsnprintf(str, LUA_MAXINPUT, fmt, arg_list); // I don't know why vfprintf(fout...) can't use when fout != stderr.
    va_end(arg_list);
    esp_lua_printf(str);
    free(str);
}

void esp_lua_completion_callback(const char *buf, linenoiseCompletions *lc) {
    if (!strcasecmp(buf,"esp")) {
        linenoiseAddCompletion(lc,"esp8266");
        linenoiseAddCompletion(lc,"esp32");
        linenoiseAddCompletion(lc,"esp32s2");
    }
}

char *esp_lua_hints_callback(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"hello")) {
        *color = 32;
        *bold = 0;
        return " world";
    }
    return NULL;
}

void esp_lua_set_dumb_mode(int mode)
{
    linenoiseSetDumbMode(mode);
}

void esp_lua_init(esp_lua_callback_t input_cb, esp_lua_callback_t output_cb, const luaL_Reg *libs)
{
    esp_lua_libs = (luaL_Reg *)libs;

    if (input_cb == NULL) {
        esp_lua_input_cb = esp_lua_input_callback_default;
    } else {
        esp_lua_input_cb = input_cb;
    }

    if (output_cb == NULL) {
        esp_lua_output_cb = esp_lua_output_callback_default;
        esp_lua_set_dumb_mode(0);
    } else {
        esp_lua_output_cb = output_cb;
        esp_lua_set_dumb_mode(1);
    }
    linenoiseSetMultiLine(1);
    linenoiseSetCompletionCallback(esp_lua_completion_callback);
    linenoiseSetHintsCallback(esp_lua_hints_callback);
    // linenoisePrintKeyCodes();
}

int esp_lua_main(int argc, char **argv)
{
    int ret = 0;
    extern int lua_main (int argc, char **argv);

    exit_flag = 0;
    linenoiseClearScreen();
    linenoiseHistoryLoad(ESP_LUA_HISTORY_PATH);
    ret = lua_main (argc, argv);
    linenoiseHistorySave(ESP_LUA_HISTORY_PATH);
    
    return ret;
}