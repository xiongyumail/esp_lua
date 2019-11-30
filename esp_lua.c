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

static int run_flag = 0;

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
    esp_lua_writestringerror("exit: %d", status);
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
    fprintf(ferr, "\r\n");
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

int esp_lua_init(FILE *in, FILE *out, FILE *err)
{
    fin = in;
    fout = out;
    ferr = err;

    return 0;
}

int esp_lua_main(int argc, char **argv)
{
    extern int lua_main (int argc, char **argv);

    run_flag = 1;
    return lua_main (argc, argv);
}