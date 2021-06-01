#include "product.h"
#include <iostream>
using namespace std;

Product::Product()
{
    cout << "Product constructor" << endl;
}
Product::~Product()
{
    cout << "Product destructor" << endl;
}
ConcreteProduct1::ConcreteProduct1()
{
    cout << "ConcreteProduct1 constructor" << endl;
}
ConcreteProduct1::~ConcreteProduct1()
{
    cout << "ConcreteProduct1 destructor" << endl;
}
ConcreteProduct3::ConcreteProduct3()
{
    cout << "ConcreteProduct3 constructor" << endl;
}
ConcreteProduct3::~ConcreteProduct3()
{
    cout << "ConcreteProduct3 destructor" << endl;
}
