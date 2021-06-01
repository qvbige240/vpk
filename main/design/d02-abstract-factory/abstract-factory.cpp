#include <iostream>
#include "product.h"
#include "abstract-factory.h"
using namespace std;

AbstractFactory::AbstractFactory()
{
    cout << "father:AbstractFactory constructor" << endl;
    pointA1 = NULL;
    pointA2 = NULL;
    pointB1 = NULL;
    pointB2 = NULL;
}
AbstractFactory::~AbstractFactory()
{
    cout << "father:AbstractFactory destructor" << endl;
    if (pointA1)
        delete pointA1;
    if (pointA2)
        delete pointA2;
    if (pointB1)
        delete pointB1;
    if (pointB2)
        delete pointB2;
}
ConcreteFactory1::ConcreteFactory1()
{
}
ConcreteFactory1::~ConcreteFactory1()
{
}
AbstractProductA *ConcreteFactory1::CreateProductA()
{
    cout << "ConcreteFactory1:new ProductA1()" << endl;
    pointA1 = new ProductA1();
    return pointA1;
}
AbstractProductB *ConcreteFactory1::CreateProductB()
{
    cout << "ConcreteFactory1:new ProductB1()" << endl;
    pointB1 = new ProductB1();
    return pointB1;
}
ConcreteFactory2::ConcreteFactory2()
{
}
ConcreteFactory2::~ConcreteFactory2()
{
}
AbstractProductA *ConcreteFactory2::CreateProductA()
{
    cout << "ConcreteFactory2:new ProductA2()" << endl;
    pointA2 = new ProductA2();
    return pointA2;
}
AbstractProductB *ConcreteFactory2::CreateProductB()
{
    cout << "ConcreteFactory2:new ProductB2()" << endl;
    pointB2 = new ProductB2();
    return pointB2;
}
