#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(LUA_MAXINPUT)
#define LUA_MAXINPUT		512
#endif

#if !defined(luaL_openlibs) 
#define luaL_openlibs esp_luaL_openlibs // Has been declared
#endif

#if !defined(system)
#define system(string)  esp_lua_system(string)
#endif

#if !defined(signal)
#define signal(sig, func)  esp_lua_signal(sig, func)
#endif

#if !defined(exit)
#define exit(status) esp_lua_exit(status)
#endif

// #if !defined(lua_lock)
// #define lua_lock(L)	esp_lua_lock()
// #define lua_unlock(L)	esp_lua_unlock()
// #endif

/* print a string */
#if !defined(lua_writestring)
#define lua_writestring(s,l)   esp_lua_writestring(s,l)
#endif

/* print a newline and flush the output */
#if !defined(lua_writeline)
#define lua_writeline()        esp_lua_writeline()
#endif

/* print an error message */
#if !defined(lua_writestringerror)
#define lua_writestringerror(s,p) esp_lua_writestringerror(s, p)
#endif

#if !defined(lua_readline)	/* { */
#include "lua_linenoise.h"
#define lua_readline(L,b,p)	((void)L, ((b)=lua_linenoise(p)) != NULL)
#define lua_saveline(L,line)	((void)L, lua_linenoiseHistoryAdd(line))
#define lua_freeline(L,b)	((void)L, lua_linenoiseFree(b))
#endif				/* } */

size_t esp_lua_read(char *ptr, size_t len);

size_t esp_lua_write(char *ptr, size_t len);

int esp_lua_fflush();

int esp_lua_printf(const char *fmt, ...);

int esp_lua_putc(int c);

int esp_lua_system(const char * string);

void (*esp_lua_signal(int sig, void (*func)(int)))(int);

void esp_lua_exit(int status);

void esp_lua_lock(void);

void esp_lua_unlock(void);

size_t esp_lua_writestring(const char *str, size_t size);

void esp_lua_writeline(void);

void esp_lua_writestringerror(const char *fmt, ...);

#ifdef __cplusplus
}
#endif