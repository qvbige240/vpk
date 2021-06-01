#include <iostream>
#include "abstraction.h"
#include "abstractionimp.h"
using namespace std;

Abstraction::Abstraction()
{
}

Abstraction::~Abstraction()
{
}

RefinedAbstraction::RefinedAbstraction(AbstractionImp *imp)
{
    _imp = imp;
}

RefinedAbstraction::~RefinedAbstraction()
{
}

void RefinedAbstraction::Operation()
{
    cout << "_imp->Operation() interface" << endl;
    _imp->Operation();
}
