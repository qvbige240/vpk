
#include <iostream>
#include "abstract-factory.h"
using namespace std;

int main()
{
    AbstractFactory *cf1 = new ConcreteFactory1();
    cf1->CreateProductA();
    cf1->CreateProductB();

    AbstractFactory *cf2 = new ConcreteFactory2();
    cf2->CreateProductA();
    cf2->CreateProductB();

    delete cf1;
    delete cf2;
    return 0;
}
