## How to use

* stdin, stdout, stderr
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
    esp_lua_main(1, ESP_LUA_ARGV);
}
```

* memstream

```c
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "esp_lua.h"

// After a successful fflush() or fclose(), the pointer referenced by bufp shall contain the address of the buffer, 
// and the variable pointed to by sizep shall contain the smaller of the current buffer length and the number of bytes for open_memstream()
FILE *fin = NULL;
char *fin_buffer = NULL;
size_t fin_size = NULL; 

FILE *fout = NULL;
char *fout_buffer = NULL;
size_t fout_size = NULL;

void app_main()
{
    const char *ESP_LUA_ARGV[2] = {"./lua", NULL};

    fin = open_memstream(&fout_buffer, &fout_size);
    fout = open_memstream(&fout_buffer, &fout_size);

    esp_lua_init(fin, fout, fout);
    esp_lua_main(1, ESP_LUA_ARGV);

    fclose(fin);
    free(fin_buffer);
    fclose(fout);
    free(fout_buffer);
}
```
