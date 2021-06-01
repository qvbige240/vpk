#include <iostream>
#include "facade.h"
using namespace std;

Facade *Facade::_instance = NULL;

Facade *Facade::GetInstance()
{
    if (!_instance)
    {
        _instance = new Facade();
    }
    return _instance;
}

void Facade::CloseInstance()
{
    if (_instance)
        delete _instance;
}

Facade::Facade()
{
    _subs1 = new Subsystem1();
    _subs2 = new Subsystem2();
}

Facade::~Facade()
{
    if (_subs1)
        delete _subs1;
    if (_subs2)
        delete _subs2;
}

void Facade::OperationWrapper()
{
    cout << "Facade::OperationWrapper" << endl;
    _subs1->Operation();
    _subs2->Operation();
}

Subsystem1::Subsystem1()
{
}

Subsystem1::~Subsystem1()
{
}

void Subsystem1::Operation()
{
    cout << "---Subsystem1 Operation..." << endl;
}
Subsystem2::Subsystem2()
{
}
Subsystem2::~Subsystem2()
{
}
void Subsystem2::Operation()
{
    cout << "---Subsystem2 Operation..." << endl;
}
