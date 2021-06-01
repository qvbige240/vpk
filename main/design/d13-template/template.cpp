#include <iostream>
#include "template.h"
using namespace std;

AbstractClass::AbstractClass()
{
}

AbstractClass::~AbstractClass()
{
}

void AbstractClass::TemplateMethod()
{
    this->PrimitiveOperation1();
    this->PrimitiveOperation2();
}

ConcreteClass1::ConcreteClass1()
{
    cout << "ConcreteClass1 constructor..." << endl;
}
ConcreteClass1::~ConcreteClass1()
{
}
void ConcreteClass1::PrimitiveOperation1()
{
    cout << "---ConcreteClass1...PrimitiveOperation1" << endl;
}
void ConcreteClass1::PrimitiveOperation2()
{
    cout << "---ConcreteClass1...PrimitiveOperation2" << endl;
}
ConcreteClass2::ConcreteClass2()
{
    cout << "ConcreteClass2 constructor..." << endl;
}
ConcreteClass2::~ConcreteClass2()
{
}
void ConcreteClass2::PrimitiveOperation1()
{
    cout << "+++ConcreteClass2...PrimitiveOperation1" << endl;
}
void ConcreteClass2::PrimitiveOperation2()
{
    cout << "+++ConcreteClass2...PrimitiveOperation2" << endl;
}

// main

int main()
{
    //ConcreteClass1与ConcreteClass2可相互替换
    AbstractClass *p1 = new ConcreteClass1();
    AbstractClass *p2 = new ConcreteClass2();
    p1->TemplateMethod();
    p2->TemplateMethod();

    delete p1;
    delete p2;
    return 0;
}
