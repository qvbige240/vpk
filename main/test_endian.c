#include <stdio.h>
#include <stdlib.h>

int check_endian()
{
    union check
    {
        int i;
        char ch;
    }c;
    c.i = 1;
    return (c.ch == 1);
}

int main(int argc, char *argv[])
{
    check_endian() == 1 ? printf("little-endian\n") : printf("big-endian\n");
    return 0;
}

