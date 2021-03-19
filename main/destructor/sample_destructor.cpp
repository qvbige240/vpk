
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "foo.h"

// g++ -g -O0 -o test *.cpp -lpthread
int main(int argc, char *argv[])
{

    FooDemo *foo = new FooDemo(NULL, NULL);
    foo->start();
    sleep(1);
    foo->stop();

    delete foo;

    while (1)
    {
        sleep(5);
    }

    return 0;
}
