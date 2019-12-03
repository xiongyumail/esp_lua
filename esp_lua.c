#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linenoise.h"
#include "esp_lua.h"

FILE *fin = NULL;
FILE *fout = NULL;
FILE *ferr = NULL;
int exit_flag = 0;
static luaL_Reg *esp_lua_libs = NULL;

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
    return fwrite((const void *)str, sizeof(char), size, fout);
}

void esp_lua_writeline(void)
{
    esp_lua_writestring("\n", 1);
    fflush(fout);
}

void esp_lua_writestringerror(const char *fmt, ...)
{
    char *str = calloc(LUA_MAXINPUT, sizeof(char));
    va_list arg_list;
    va_start(arg_list, fmt);
    vsnprintf(str, LUA_MAXINPUT, fmt, arg_list); // I don't know why vfprintf(fout...) can't use when fout != stderr.
    va_end(arg_list);
    fprintf(ferr, "\n\n");
    fprintf(ferr, str);
    fflush(ferr);
    free(str);
}

void completion(const char *buf, linenoiseCompletions *lc) {
    if (!strcasecmp(buf,"mylib")) {
        linenoiseAddCompletion(lc,"mylib.hello()");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"mylib")) {
        *color = 32;
        *bold = 0;
        return ".hello()";
    }
    return NULL;
}

void esp_lua_init(FILE *in, FILE *out, FILE *err, const luaL_Reg *libs)
{
    fin = in;
    fout = out;
    ferr = err;
    esp_lua_libs = (luaL_Reg *)libs;
    linenoiseSetMultiLine(1);
    // linenoiseSetDumbMode(1);
    // linenoisePrintKeyCodes();
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);
}

int esp_lua_main(int argc, char **argv)
{
    int ret = 0;
    extern int lua_main (int argc, char **argv);

    exit_flag = 0;
    linenoiseClearScreen();
    linenoiseHistoryLoad("/lua/history.txt");
    ret = lua_main (argc, argv);
    linenoiseHistorySave("/lua/history.txt");
    return ret;
}