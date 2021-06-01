#include <iostream>
#include "colleague.h"
#include "mediator.h"
using namespace std;

Colleague::Colleague()
{
}

Colleague::Colleague(Mediator *mdt)
{
    _mdt = mdt;
}

Colleague::~Colleague()
{
}

ConcreteColleageA::ConcreteColleageA()
{
}

ConcreteColleageA::~ConcreteColleageA()
{
}

ConcreteColleageA::ConcreteColleageA(Mediator *mdt)
    : Colleague(mdt)
{
}

string ConcreteColleageA::GetState()
{
    return _sdt;
}

void ConcreteColleageA::SetState(const string &sdt)
{
    _sdt = sdt;
}

void ConcreteColleageA::Action()
{
    _mdt->DoActionFromAtoB();
    cout << "---State of ConcreteColleageA:" << this->GetState() << endl;
}

ConcreteColleageB::ConcreteColleageB()
{
}
ConcreteColleageB::~ConcreteColleageB()
{
}
ConcreteColleageB::ConcreteColleageB(Mediator *mdt) : Colleague(mdt)
{
}
void ConcreteColleageB::Action()
{
    _mdt->DoActionFromBtoA();
    cout << "---State of ConcreteColleageB:" << this->GetState() << endl;
}
string ConcreteColleageB::GetState()
{
    return _sdt;
}
void ConcreteColleageB::SetState(const string &sdt)
{
    _sdt = sdt;
}
