#include <iostream>
#include "facade.h"
using namespace std;

int main()
{
    Facade *fd = Facade::GetInstance();
    fd->OperationWrapper();

    Facade::CloseInstance();
    return 0;
}
