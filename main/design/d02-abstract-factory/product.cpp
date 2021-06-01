#include <iostream>
#include "product.h"
using namespace std;

AbstractProductA::AbstractProductA()
{
}

AbstractProductA::~AbstractProductA()
{
}

AbstractProductB::AbstractProductB()
{
}

AbstractProductB::~AbstractProductB()
{
}

ProductA1::ProductA1()
{
    cout << "---ProductA1 constructor" << endl;
}
ProductA1::~ProductA1()
{
    cout << "---ProductA1 destructor" << endl;
}
ProductA2::ProductA2()
{
    cout << "---ProductA2 constructor" << endl;
}
ProductA2::~ProductA2()
{
    cout << "---ProductA2 destructor" << endl;
}
ProductB1::ProductB1()
{
    cout << "+++ProductB1 constructor" << endl;
}
ProductB1::~ProductB1()
{
    cout << "+++ProductB1 destructor" << endl;
}
ProductB2::ProductB2()
{
    cout << "+++ProductB2 constructor" << endl;
}
ProductB2::~ProductB2()
{
    cout << "+++ProductB2 destructor" << endl;
}
