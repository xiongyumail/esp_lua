#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(LUA_MAXINPUT)
#define LUA_MAXINPUT		512
#endif

#if !defined(system)
#define system(string)  esp_lua_system(string)
#endif

#if !defined(signal)
#define signal(sig, func)  esp_lua_signal(sig, func)
#endif

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
#define lua_writestringerror(s,p) 
#endif

#if !defined(lua_readline)	/* { */

#define lua_readline(L,b,p)	((void)L, ((b)=esp_lua_readline(p)) != NULL)
#define lua_saveline(L,line)	((void)L, esp_lua_add_history(line))
#define lua_freeline(L,b)	((void)L, esp_lua_free(b))

#endif				/* } */

int esp_lua_system(const char * string);

void (*esp_lua_signal(int sig, void (*func)(int)))(int);

size_t esp_lua_writestring(const char *str, size_t size);

void esp_lua_writeline(void);

char *esp_lua_readline(const char *prompt);

void esp_lua_add_history(const char *string);

void esp_lua_free(void *ptr);

int esp_lua_main(void);

int esp_lua_init(FILE *in, FILE *out);

#ifdef __cplusplus
}
#endif