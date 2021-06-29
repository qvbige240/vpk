#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugin_dl.h"

int main(int argc, char *argv[])
{
    int result = 0;
    const char *path = "./libadd.so";
    plugin_func_t *thiz = (plugin_func_t *)calloc(1, sizeof(plugin_func_t));
    if (thiz)
    {
        plugin_load(thiz, path);
        int a = 23, b = 2;
        result = thiz->add(a, b);
        printf("%d add %d: %d\n", a, b, result);
        result = thiz->sub(a, b);
        printf("%d sub %d: %d\n", a, b, result);
        result = thiz->mul(a, b);
        printf("%d mul %d: %d\n", a, b, result);
    }
    return 0;
}
