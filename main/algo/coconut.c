#include <stdio.h>
#include <stdlib.h>

typedef enum {false, true} boolean;

void coconut(void)
{
    int i, j, m, x;
    boolean flag;
    for (i = 0; i < 100000; ++i)
    {
        m = i;
        flag = true;
        //均满足6次与5求余且余1
        for (j = 0; j < 6; ++j)
        {
            if ((m % 5) != 1)
            {
                flag = false;
                break;
            }
            m = m - (1 + m / 5);
        }
        if (!flag)
        {
            continue;
        }
        //输出在100000以内，满足6次与5求余且余数都为1的数和最后一次除5得的数
        m = i;
        for (j = 0; j < 6; ++j)
        {
            printf("%d/5 -> (%d) ", m, m/5);
            x = m / 5;
            m = m - (1 + m / 5);
            printf("%d --- 1\n", m);
        }
        printf("%s%5d%s%5d\n", "椰子的总个数为：", i, " 最后一次每人分得的个数：", x);
    }
}

int main(int argc, char *argv[])
{
    coconut();

    return 0;
}
