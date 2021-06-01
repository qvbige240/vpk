#include <iostream>
#include "flyweight.h"
using namespace std;

Flyweight::Flyweight(string intrinsicState)
{
    _intrinsicState = intrinsicState;
}

Flyweight::~Flyweight()
{
}

void Flyweight::Operation(const string &extrinsicState)
{
}

string Flyweight::GetIntrinsicState()
{
    return this->_intrinsicState;
}

ConcreteFlyweight::ConcreteFlyweight(string intrinsicState)
    : Flyweight(intrinsicState)
{
    cout << "ConcreteFlyweight Build..." << intrinsicState << endl;
}
ConcreteFlyweight::~ConcreteFlyweight()
{
}
void ConcreteFlyweight::Operation(const string &extrinsicState)
{
    cout << "ConcreteFlyweight:内部[" << GetIntrinsicState() << "]外部[" << extrinsicState << "]" << endl;
}
UnsharedConcreteFlyweight::UnsharedConcreteFlyweight(string intrinsicState)
    : Flyweight(intrinsicState)
{
}
UnsharedConcreteFlyweight::~UnsharedConcreteFlyweight()
{
}
void UnsharedConcreteFlyweight::Operation(const string &extrinsicState)
{
    cout << "---extrinsicState:" << extrinsicState << endl;
}
