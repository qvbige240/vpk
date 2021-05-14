#include <stdio.h>
#include <stdlib.h>

int binary_print(int a)
{
    if (a > 0)
    {
        binary_print(a >> 1);
        printf("%d", a % 2);
    }
}

int binary_print_2(int a)
{
    int i = 0, k = 0;
    unsigned char *p = (unsigned char*)&a + 3;
    for (k = 0; k < 4; k++)
    {
        int val = *(p - k);
        for (i = 7; i >= 0; i--)
        {
            if (val & (1 << i))
                printf("1");
            else
                printf("0");
        }
    }
}

int test_binary_print()
{
    int a = 13;
    printf("%d binary: ", a);
    binary_print(a);
    printf("\n");

    a = 15;
    printf("%d binary: ", a);
    binary_print_2(a);
    printf("\n\n");
    return 0;
}

/** bit add without +-*/ **/
int add_binary(int a, int b)
{
    int x = a, y = b, answer, carry;
    while (y)
    {
        answer = x ^ y;
        carry = (x & y) << 1;
        x = answer;
        y = carry;
    }
    return x;
}
int bit_add(int a, int b)
{
    int carry = a & b;
    int no_carray_sum = a ^ b;

    // binary_print_2(a);
    // printf("\n");
    // binary_print_2(b);
    // printf("\n");
    // printf("---------\n");
    // binary_print_2(carry);
    // printf("\n");
    // binary_print_2(no_carray_sum);
    // printf("\n\n");

    if (carry)
        return bit_add(carry << 1, no_carray_sum);
    else
        return no_carray_sum;    
}

int bit_multiply7(int a)
{
    //bit_add(a << 1, a);
    bit_add(a << 2, bit_add(a << 1, a));
}

int test_multiply7()
{
    int a = 13;

    int value = bit_multiply7(a);

    printf("%d multiply 7: %d\n", a, value);

    printf("\n");

    return 0;
}

/** greatest common divisor **/
int gcd(int a, int b)
{
    return b ? gcd(b, a % b) : a;
}

int test_gcd()
{
    int a = 16, b = 12;

    int val = gcd(a, b);

    printf("%d and %d greatest common divisor: %d\n", a, b, val);

    printf("\n");
}

int main(int argc, char *argv[])
{
    test_multiply7();
    test_binary_print();
    test_gcd();

    return 0;
}

