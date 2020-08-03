#include <stdio.h>
#include <stdlib.h>

void get_next(char *p, int next[])
{
    int len = strlen(p);
    next[0] = -1;
    int k = -1;
    int j = 0;
    while (j < len - 1)
    {
        // p[k]表示前缀，p[j]表示后缀
        if (k == -1 || p[j] == p[k])
        {
            ++k;
            ++j;
            next[j] = k;
        }
        else
        {
            k = next[k];
        }
    }
}

int main(int argc, char *argv[])
{
    return 0;
}
