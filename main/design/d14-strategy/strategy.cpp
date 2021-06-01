#include <iostream>
#include "strategy.h"
using namespace std;

Strategy::Strategy()
{
    cout << "Strategy constructor" << endl;
}
Strategy::~Strategy()
{
    cout << "Strategy destructor" << endl;
}
void Strategy::AlgrithmInterface()
{
}

ConcreteStrategyA::ConcreteStrategyA()
{
    cout << "---ConcreteStrategyA constructor" << endl;
}
ConcreteStrategyA::~ConcreteStrategyA()
{
    cout << "---ConcreteStrategyA destructor" << endl;
}
void ConcreteStrategyA::AlgrithmInterface()
{
    cout << "---ConcreteStrategyA::AlgrithmInterface" << endl;
}

ConcreteStrategyB::ConcreteStrategyB()
{
    cout << "+++ConcreteStrategyB constructor" << endl;
}
ConcreteStrategyB::~ConcreteStrategyB()
{
    cout << "+++ConcreteStrategyB destructor" << endl;
}
void ConcreteStrategyB::AlgrithmInterface()
{
    cout << "+++ConcreteStrategyB::AlgrithmInterface" << endl;
}
