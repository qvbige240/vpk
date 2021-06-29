#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int add(int a, int b);
int sub(int a, int b);
int mul(int a, int b);

#ifdef __cplusplus
}
#endif

/** gcc -fPIC -shared -g -o libadd.so add.c **/
int add(int a, int b)
{
    return a + b;
}
int sub(int a, int b)
{
    return a - b;
}
int mul(int a, int b)
{
    return a * b;
}
