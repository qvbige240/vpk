#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 
// void get_next(const char *p, int next[])
// {
//     int len = strlen(p);
//     next[0] = -1;
//     int k = -1;
//     int j = 0;
//     while (j < len - 1)
//     {
//         // p[k]表示前缀，p[j]表示后缀
//         if (k == -1 || p[j] == p[k])
//             next[++j] = ++k;
//         else
//             k = next[k];
//     }
// }

static void get_next(const char *p, int next[])
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
            if (p[j + 1] == p[k + 1])
                next[++j] = next[++k];
            else
                next[++j] = ++k;
        }
        else
        {
            k = next[k];
        }
    }
}
static int my_strstr(const char *haystack, const char *needle)
{
    if (!haystack) return -1;
    if (!needle) return 0;

    int len = strlen(needle);
    int i = 0, j = 0;
    int next[len];

    get_next(needle, next);
    for (i = 0; i < len; i++)
        printf("%d%s", next[i], i == len - 1 ? "\n" : ", ");

    i = 0;
    j = 0;
    while (haystack[i] && j < len)
    {
        if (j == -1 || haystack[i] == needle[j])
        {
            j++;
            i++;
        }
        else
        {
            j = next[j];
        }
    }

    if (j == len)
    {
        i = i - j;
        return i;
    }

    return -1;
}
int main(int argc, char *argv[])
{
    const char *needle = "cbacac";
    const char *str = "acbaccabacac";
    // // char *str = " 000121";
    // int i = 0;
    // int next[10] = {0};

    // get_next(str, next);
    // for (i = 0; i < strlen(str); i++)
    //     printf("%d%s", next[i], i == strlen(str)-1 ? "\n" : ", ");

    // get_next_optimize(str, next);
    // for (i = 0; i < strlen(str); i++)
    //     printf("%d%s", next[i], i == strlen(str)-1 ? "\n" : ", ");

    int pos = my_strstr(str, needle);
    printf(" %s in %s at %d\n", needle, str, pos);
    return 0;
}
