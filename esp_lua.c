#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lua.h"

static FILE *fin = NULL;
static FILE *fout = NULL;
static FILE *ferr = NULL;
static luaL_Reg *esp_lua_libs = NULL;

static int run_flag = 0;

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

size_t esp_lua_fread(char *str, size_t size, size_t n, FILE *f)
{
    char c[2] = {0};

    if (str == NULL || f == NULL || size * n == 0) {
        return 0;
    }

    // Block until the corresponding number of characters is obtained
    for (int x = 0; x < size * n;) {
        if (fread(c, sizeof(char), 1, f) <= 0) {
            str[x++] = c[0];
        } else {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    str[x] = '\0';

    return x;
};

char *esp_lua_fgets(char *str, int n, FILE *f) 
{
    if (str == NULL || n <= 0) {
        return NULL;
    }
    
    // Block until the corresponding number of characters is obtained
    for (int x = 0; x < n;) {
        if (fread(c, sizeof(char), 1, f) <= 0) {
            if (c[0] == '\n') {
                break;
            }
            str[x++] = c[0];
        } else {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    str[x] = '\0';
    
    return str;
}

int	esp_lua_fgetc(FILE *f) 
{
    char *str[2] = {0};
    esp_lua_fread(str, sizeof(char), 1, f);
    return (int)str[0];
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
    run_flag = 0;
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
    vsnprintf(str, LUA_MAXINPUT, fmt, arg_list); // I don't know why vfprinf(fout...) can't use when fout != stderr.
    va_end(arg_list);
    fprintf(ferr, "\n\n");
    fprintf(ferr, str);
    fflush(ferr);
    free(str);
}

char *esp_lua_fgets(char *str, int n, FILE *f) 
{
    char str_read[2] = {0};
    while (1) {
        if (fread(str_read, sizeof(char), 1, f) != 0) {
            if (strchr(str_read, '\b') != NULL) {
                if (strlen(str) != 0) { // Do not del too much
                    fputs("\b \b", fout);
                    fflush(fout);
                    str[strlen(str)-1] = '\0';// del char
                }
            } else {
                fputs(str_read, fout);
                fflush(fout);
                if (strlen(str) + strlen(str_read) +2 >= n) { // can't too long.
                    free(str);
                    return NULL;
                }
                strcat(str, str_read);
                if (strchr(str_read, '\n') != NULL) {
                    // need 2 '\n'
                    fputs("\r\n", fout);
                    fflush(fout);
                    break;
                }
            }
        } else if (!run_flag) {
            return NULL;
        } else {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    return str;
}

char *esp_lua_readline(const char *prompt)
{
    char *str = calloc(LUA_MAXINPUT, sizeof(char));

    fputs(prompt, fout); /* show prompt */
    fflush(fout);

    return esp_lua_fgets(str, LUA_MAXINPUT, fin);
}

void esp_lua_add_history(const char *string)
{
}

void esp_lua_free(void *ptr)
{
    free(ptr);
}

void esp_lua_init(FILE *in, FILE *out, FILE *err, const luaL_Reg *libs)
{
    fin = in;
    fout = out;
    ferr = err;
    esp_lua_libs = (luaL_Reg *)libs;
}

int esp_lua_main(int argc, char **argv)
{
    extern int lua_main (int argc, char **argv);

    run_flag = 1;
    return lua_main (argc, argv);
}