## How to use

```c
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "esp_lua.h"

void app_main()
{
    esp_lua_init(stdin, stdout);
    esp_lua_main();
}
```
