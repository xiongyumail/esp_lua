## _ESP_LUA_

Very pure Lua library, I only added the `esp_lua.h` header file to `luaconf.h`

I have implemented a simple terminal, but it is not perfect. For example, history has not been implemented.

I also want to use `linenose`, but I'm too `vegetable` to use it. :（

## How to use

* stdin, stdout, stderr

The most commonly used method, but only through the serial port input and output.

```c
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "esp_lua.h"

void app_main()
{
    const char *ESP_LUA_ARGV[2] = {"./lua", NULL};
    esp_lua_init(stdin, stdout, stderr);
    while (1) {
        // Clear monitor screen
        fprintf(stdout,"\x1b[H\x1b[2J");
        esp_lua_main(1, ESP_LUA_ARGV);
    }
}
```

* fmem

Here file memory I/O stream is bound to STD, just to show how to use it, you can also map it to other devices, such as LCD.

```c
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "esp_lua.h"

FILE *fin = NULL;
FILE *fout = NULL;
FILE *ferr = NULL;
char *fin_buffer = NULL;
char *fout_buffer = NULL;
char *ferr_buffer = NULL;

void lua_task(void *arg)
{
    char *ESP_LUA_ARGV[2] = {"./lua", NULL};

    fin_buffer  = (char *)calloc(sizeof(char) + 1, sizeof(char)); // We need check the character one by one.
    fout_buffer = (char *)calloc(LUA_MAXINPUT + 1, sizeof(char));
    ferr_buffer = (char *)calloc(LUA_MAXINPUT + 1, sizeof(char));
    fin  = fmemopen(fin_buffer,  sizeof(char), "r");
    fout = fmemopen(fout_buffer, LUA_MAXINPUT, "w");
    ferr = fmemopen(ferr_buffer, LUA_MAXINPUT, "w");
    
    esp_lua_init(fin, fout, ferr);

    while (1) {
        // Clear monitor screen
        fprintf(stdout,"\x1b[H\x1b[2J");
        esp_lua_main(1, ESP_LUA_ARGV);
    }
    

    fclose(fin);
    free(fin_buffer);
    fclose(fout);
    free(fout_buffer);
    fclose(ferr);
    free(ferr_buffer);

    vTaskDelete(NULL);
}

void stream_task(void *arg)
{
    char c[2];

    while (1) { 
        if (ferr && ftell(ferr)) {
            fprintf(stderr, ferr_buffer);
            rewind (ferr);
        }

        if (fout && ftell(fout)) {
            fprintf(stdout, fout_buffer);
            rewind (fout);
        }

        if (fin && fread(c, sizeof(char), 1, stdin) != 0) {
            fin_buffer[0] = c[0];
            fin_buffer[1] = '\0';
            rewind (fin);
            while (1) { // Wait for Lua to complete input
                if (ftell(fin) == 1) {
                    break;
                } else {
                    vTaskDelay(10 / portTICK_RATE_MS);
                }
            }
        }
        
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void app_main()
{
    xTaskCreate(stream_task, "stream_task", 4096, NULL, 5, NULL);
    xTaskCreate(lua_task, "lua_task", 4096, NULL, 5, NULL);
}
```
