
#include "product.h"
#include "factory.h"
#include <iostream>
using namespace std;

Factory::Factory() : _flag(0)
{
}

Factory::~Factory()
{
}

void Factory::setFactoryMethod(int flag)
{
    _flag = flag;
}

ConcreteFactory::ConcreteFactory()
{
    cout << "---ConcreteFactory constructor" << endl;
}
ConcreteFactory::~ConcreteFactory()
{
    cout << "---ConcreteFactory destructor" << endl;
}
Product *ConcreteFactory::CreateProduct() //创建操作
{
    if (_flag == 1)
    {
        return new ConcreteProduct1(); //子类中决定要实例化哪一个类
    }
    else if (_flag == 3)
    {
        return new ConcreteProduct3();
    }
}
