#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "plugin_dl.h"

int plugin_load(plugin_func_t *plugin, const char *dll_path)
{
    void *handle = dlopen(dll_path, RTLD_LAZY);
    if (!handle)
    {
        fprintf(stderr, "[%s](%d) dlopen get error: %s\n", __FILE__, __LINE__, dlerror());
        return -1;
    }

    plugin->handle  = handle;
    plugin->add     = (PLUGIN_FUNC_ADD)dlsym(handle, "add");
    plugin->sub     = (PLUGIN_FUNC_SUB)dlsym(handle, "sub");
    plugin->mul     = (PLUGIN_FUNC_MUL)dlsym(handle, "mul");

    // dlclose(handle);
    return 0;
}
