
#include "factory.h"
#include "product.h"
#include <iostream>
using namespace std;

int main()
{
    Factory *factory = new ConcreteFactory();

    // ConcreteFactory延时通过参数决定到底创建具体哪一个Product的子类
    factory->setFactoryMethod(3);
    Product *pro = factory->CreateProduct();

    if (pro)
    {
        delete pro;
        pro = NULL;
    }
    if (factory)
    {
        delete factory;
        factory = NULL;
    }
    return 0;
}
