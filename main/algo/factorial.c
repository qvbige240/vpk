#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// #include <memory.h>

// #include "util_uint128.h"

int factorial_operator(int big[], int len, int multiple)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        big[i] = big[i] * multiple;
    }

    for (i = 0; i < len - 1; i++)
    {
        big[i + 1] += big[i] / 10;
        big[i] = big[i] % 10;
    }

    return 0;
}

void factorial_big(int n)
{
    int big_data[1000] = {0};
    // int num = 50;
    big_data[0] = 1;

    int i = 0;
    for (i = 1; i <= n; i++)
    {
        factorial_operator(big_data, 1000, i);
    }

    // print
    int k = 0;
    for (i = 1000 - 1; i >= 0; i--)
    {
        if (big_data[i] != 0)
        {
            k = i;
            break;
        }
    }
    for (i = k; i >= 0; i--)
    {
        printf("%d", big_data[i]);
    }
    printf(" \n");
}

int main()
{
    int n = 0;
    printf("input: \n");
    scanf("%d", &n);

    factorial_big(n);
}
