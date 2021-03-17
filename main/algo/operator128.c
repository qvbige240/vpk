#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#include "util_uint128.h"

char *Bin128ToDec(uint32_t N[4])
{
    // log10(x) = log2(x) / log2(10) ~= log2(x) / 3.322
    static char s[128 / 3 + 1 + 1];
    uint32_t n[4];
    char *p = s;
    int i;

    memset(s, '0', sizeof(s) - 1);
    s[sizeof(s) - 1] = '\0';

    memcpy(n, N, sizeof(n));

    for (i = 0; i < 128; i++)
    {
        int j, carry;

        carry = (n[3] >= 0x80000000);
        // Shift n[] left, doubling it
        n[3] = ((n[3] << 1) & 0xFFFFFFFF) + (n[2] >= 0x80000000);
        n[2] = ((n[2] << 1) & 0xFFFFFFFF) + (n[1] >= 0x80000000);
        n[1] = ((n[1] << 1) & 0xFFFFFFFF) + (n[0] >= 0x80000000);
        n[0] = ((n[0] << 1) & 0xFFFFFFFF);

        // Add s[] to itself in decimal, doubling it
        for (j = sizeof(s) - 2; j >= 0; j--)
        {
            s[j] += s[j] - '0' + carry;

            carry = (s[j] > '9');

            if (carry)
            {
                s[j] -= 10;
            }
        }
    }

    while ((p[0] == '0') && (p < &s[sizeof(s) - 2]))
    {
        p++;
    }

    return p;
}

static void binary_print_32(uint32_t a)
{
    int i = 0, k = 0;
    unsigned char *p = (unsigned char *)&a + 3;
    for (k = 0; k < 4; k++)
    {
        unsigned char val = *(p - k);
        for (i = 7; i >= 0; i--)
        {
            if (val & (1 << i))
                printf("1");
            else
                printf("0");
        }
        printf(" ");
    }
}

void binary_print_128(uint32_t i32[])
{
    int i = 0;
    uint32_t *p = i32 + 3;
    for (i = 0; i < 4; i++)
    {
        uint32_t val = *(p - i);
        binary_print_32(val);
        printf("  ");
    }

    printf("\n");
}

static void decimal_string_32(uint32_t a)
{
    int i = 0, j = 0;
    int t = 0, bit = 0;
    int n[1000] = {0};
    int str[1000] = {0};
    // unsigned char *p = (unsigned char*)&a + 3;
    unsigned char *p = (unsigned char *)&a;

    n[0] = 1;
    for (bit = 0; bit < 32; bit++)
    {
        unsigned char val = *(p + bit / 8);

        if (val & (1 << (bit % 8)))
        {
            for (i = t; i < bit; i++)
            {
                // 2^n
                for (j = 0; j < 1000; j++)
                    n[j] <<= 1;
                // n[j] = 2 * n[j];

                for (j = 0; j < 1000; j++)
                {
                    if (n[j] >= 10)
                    {
                        n[j] = n[j] - 10;
                        n[j + 1] = n[j + 1] + 1;
                    }
                }
            }

            t = bit;

            for (j = 0; j < 1000; j++)
            {
                str[j] = str[j] + n[j];
                if (str[j] >= 10)
                {
                    str[j] = str[j] - 10;
                    str[j + 1] = str[j + 1] + 1;
                }
            }
        }
    }

    int k = 0;
    for (i = 1000 - 1; i >= 0; i--)
    {
        if (str[i] != 0)
        {
            k = i;
            break;
        }
    }
    for (i = k; i >= 0; i--)
    {
        printf("%d", str[i]);
    }
    printf(" \n");
}

static void decimal_string_64(uint64_t a)
{
    int i = 0, j = 0;
    int t = 0, bit = 0;
    int n[1000] = {0};
    int str[1000] = {0};
    // unsigned char *p = (unsigned char*)&a + 3;
    unsigned char *p = (unsigned char *)&a;

    n[0] = 1;
    for (bit = 0; bit < 64; bit++)
    {
        unsigned char val = *(p + bit / 8);

        if (val & (1 << (bit % 8)))
        {
            for (i = t; i < bit; i++)
            {
                // 2^bit
                for (j = 0; j < 1000; j++)
                    n[j] <<= 1;
                // n[j] = 2 * n[j];

                for (j = 0; j < 1000; j++)
                {
                    if (n[j] >= 10)
                    {
                        n[j] = n[j] - 10;
                        n[j + 1] = n[j + 1] + 1;
                    }
                }
            }

            t = bit;

            for (j = 0; j < 1000; j++)
            {
                str[j] = str[j] + n[j];
                if (str[j] >= 10) // decimal
                {
                    str[j] = str[j] - 10;
                    str[j + 1] = str[j + 1] + 1;
                }
            }
        }
    }

    // print
    int k = 0;
    for (i = 1000 - 1; i >= 0; i--)
    {
        if (str[i] != 0)
        {
            k = i;
            break;
        }
    }
    for (i = k; i >= 0; i--)
    {
        printf("%d", str[i]);
    }
    printf(" \n");
}

static void decimal_string_128(uint32_t i32[])
{
    int i = 0, j = 0;
    int t = 0, bit = 0;
    int n[1000] = {0};
    int str[1000] = {0};
    // unsigned char *p = (unsigned char*)&a + 3;
    unsigned char *p = (unsigned char *)i32;

    n[0] = 1;
    for (bit = 0; bit < 128; bit++)
    {
        unsigned char val = *(p + bit / 8);

        if (val & (1 << (bit % 8)))
        {
            for (i = t; i < bit; i++)
            {
                // 2^bit
                for (j = 0; j < 1000; j++)
                    n[j] <<= 1;
                // n[j] = 2 * n[j];

                for (j = 0; j < 1000; j++)
                {
                    if (n[j] >= 10)
                    {
                        n[j] = n[j] - 10;
                        n[j + 1] = n[j + 1] + 1;
                    }
                }
            }

            t = bit;

            for (j = 0; j < 1000; j++)
            {
                str[j] = str[j] + n[j];
                if (str[j] >= 10) // decimal
                {
                    str[j] = str[j] - 10;
                    str[j + 1] = str[j + 1] + 1;
                }
            }
        }
    }

    // print
    int k = 0;
    for (i = 1000 - 1; i >= 0; i--)
    {
        if (str[i] != 0)
        {
            k = i;
            break;
        }
    }
    for (i = k; i >= 0; i--)
    {
        printf("%d", str[i]);
    }
    printf(" \n");
}

/* it can do < 34 factorial */
util_uint128_t factorial(uint32_t n)
{
    uint32_t i = 0;
    util_uint128_t sum = {0};
    sum.low = 1;
    for (i = 1; i <= n; i++)
    {
        sum = util_mul128_32(sum, i);
        // sum = sum * i;
    }

    printf("sum high = 0x%lX, low = 0x%lX\n", sum.high, sum.low);
    // printf("sum high = %llu, low = %llu\n", sum.high, sum.low);

    // printf("%u! = %s\n", n, Bin128ToDec(sum.i32));

    binary_print_128(sum.i32);

    decimal_string_128(sum.i32);

    return sum;
}

int main()
{
    int n = 0;
    printf("input: ");
    scanf("%d", &n);

    /* it can do less than 34 factorial */
    factorial(n);

    printf(" \n");
    printf("multi 64: \n");
    util_uint128_t val = util_mul64_64(123, 456);

    binary_print_128(val.i32);
    decimal_string_128(val.i32);

    return 0;
}
