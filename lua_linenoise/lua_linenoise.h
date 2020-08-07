/* lua_linenoise.h -- VERSION 1.0
 *
 * Guerrilla line editing library against the idea that a line editing lib
 * needs to be 20,000 lines of C code.
 *
 * See lua_linenoise.c for more information.
 *
 * ------------------------------------------------------------------------
 *
 * Copyright (c) 2010-2014, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2013, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __lua_linenoise_H
#define __lua_linenoise_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_linenoiseCompletions {
  size_t len;
  char **cvec;
} lua_linenoiseCompletions;

typedef void(lua_linenoiseCompletionCallback)(const char *, lua_linenoiseCompletions *);
typedef char*(lua_linenoiseHintsCallback)(const char *, int *color, int *bold);
typedef void(lua_linenoiseFreeHintsCallback)(void *);
void lua_linenoiseSetCompletionCallback(lua_linenoiseCompletionCallback *);
void lua_linenoiseSetHintsCallback(lua_linenoiseHintsCallback *);
void lua_linenoiseSetFreeHintsCallback(lua_linenoiseFreeHintsCallback *);
void lua_linenoiseAddCompletion(lua_linenoiseCompletions *, const char *);

char *lua_linenoise(const char *prompt);
void lua_linenoiseFree(void *ptr);
void lua_linenoiseExit(void);
int lua_linenoiseHistoryAdd(const char *line);
int lua_linenoiseHistorySetMaxLen(int len);
int lua_linenoiseHistorySave(const char *filename);
int lua_linenoiseHistoryLoad(const char *filename);
void lua_linenoiseHistoryFree(void);
void lua_linenoiseClearScreen(void);
void lua_linenoiseSetMultiLine(int ml);
void lua_linenoiseSetDumbMode(int set);
void lua_linenoisePrintKeyCodes(void);

#ifdef __cplusplus
}
#endif

#endif /* __lua_linenoise_H */
