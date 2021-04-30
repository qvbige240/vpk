#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// #include <memory.h>

// #include "util_uint128.h"

void factorial_operator(int big[], int len, int multiple)
{
    int i = 0;
    for (i = 0; i < len; i++)
        big[i] = big[i] * multiple;

    for (i = 0; i < len - 1; i++)
    {
        big[i + 1] += big[i] / 10;
        big[i] = big[i] % 10;
    }
}

void factorial_big(int n)
{
    int big_data[1000] = {0};
    // int num = 50;
    big_data[0] = 1;
    int i;
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
    printf("factorial_big %d! = ", n);
    for (i = k; i >= 0; i--)
    {
        printf("%d", big_data[i]);
    }
    printf(" \n");
}

int factorial_little(int n)
{
    int sum = 1;
    for (int i = 1; i <= n; i++)
        sum = sum * i;
    return sum;
}
void factorial_little_printf(int n)
{
    printf("factorial_little: ");
    for (int i = 1; i <= n; i++)
        printf("%d!=%d ", i, factorial_little(i));
    printf("\n");
    for (int i = 1; i <= n; i++)
        printf("%d, ", factorial_little(i));
    printf("\n");
}

#define MAX_CATALAN_NUM 26
int catalan_num(int n)
{
    int C[MAX_CATALAN_NUM][MAX_CATALAN_NUM];
    for (int i = 1; i <= 2 * n; ++i)
    {
        C[i][0] = C[i][i] = 1;
        for (int j = 1; j < i; ++j)
        {
            C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
        }
    }

    // int result = C[2*n][n]/(n+1);
    return C[2 * n][n] / (n + 1);
}
int catalan_num_printf(int n)
{
    int i = 0;
    printf("catalan number: ");
    for (i = 1; i <= n; i++)
    {
        printf("[%d]%d%s", i, catalan_num(i), i == n ? "\n" : ", ");
    }
    printf("catalan number: ");
    for (i = 1; i <= n; i++)
    {
        printf("%d%s", catalan_num(i), i == n ? "\n" : ", ");
    }
    return 0;
}

int main()
{
    int n = 0;
    printf("input: \n");
    scanf("%d", &n);
    int size = 5;
    int **array = calloc(1, size * sizeof(int*));
    // array[0][] = {2, 3, 6};
    // array[1][] = {5, 6, 9};
    int p[] = {90, 1, 3};
    array[0] = p;
    printf("sizeof(array[0])=%ld %ld %d\n", sizeof(array[0]), sizeof(array[0][0]), array[0][0]);

    factorial_big(n);
    factorial_little_printf(n);
    // printf("catalan_num: %d\n", catalan_num(n));
    catalan_num_printf(n);
}
