#ifndef PLUGIN_DL_H
#define PLUGIN_DL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*PLUGIN_FUNC_ADD)(int, int);
typedef int (*PLUGIN_FUNC_SUB)(int, int);
typedef int (*PLUGIN_FUNC_MUL)(int, int);
typedef int (*PLUGIN_FUNC_DIV)(int, int);

typedef struct plugin_func_s
{
    void *handle;
    PLUGIN_FUNC_ADD add;
    PLUGIN_FUNC_SUB sub;
    PLUGIN_FUNC_MUL mul;
    PLUGIN_FUNC_DIV div;
} plugin_func_t;

int plugin_load(plugin_func_t *plugin, const char *dll_path);

#ifdef __cplusplus
}
#endif

#endif // PLUGIN_DL_H
