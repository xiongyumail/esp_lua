#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lua.h"

FILE *fin = NULL;
FILE *fout = NULL;

const static char *ESP_LUA_ARGV[1] = {"./lua"};

char lua_str[LUA_MAXINPUT] = {0};

int esp_lua_system(const char * string)
{
    printf(string);
    return 0;
}

void (*esp_lua_signal(int sig, void (*func)(int)))(int)
{
    return 0;
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
    int n;
    va_list arg_list;
    va_start(arg_list,fmt);
    n = fprintf(fout,fmt,arg_list);
    va_end(arg_list);
}

char *esp_lua_fgets(char *str, int n, FILE *f) 
{
    char str_read[2] = {0};

    while (1) {
        if (fread(str_read, sizeof(char), 1, f) != 0) { // read char one by one, because esp can't block read.
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
                    return NULL;
                }
                strcat(str, str_read);
                if (strchr(str_read, '\n') != NULL) {
                    // need 2 '\n'
                    fputs("\n", fout);
                    fflush(fout);
                    break;
                }
            }
        } else {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    return str;
}

char *esp_lua_readline(const char *prompt)
{
    char str_read[2] = {0};
    char *str = lua_str;

    memset(str, 0, LUA_MAXINPUT);

    fputs(prompt, fout); /* show prompt */
    fflush(fout);

    return esp_lua_fgets(str, LUA_MAXINPUT, fin);
}

void esp_lua_add_history(const char *string)
{

}

void esp_lua_free(void *ptr)
{

}

int esp_lua(FILE *in, FILE *out)
{
    fin = in;
    fout = out;

    extern int lua_main (int argc, char **argv);
    return lua_main (1, ESP_LUA_ARGV);
}