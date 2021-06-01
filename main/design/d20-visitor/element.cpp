#include <iostream>
#include "element.h"
#include "visitor.h"
using namespace std;

Element::Element()
{
}

Element::~Element()
{
}

void Element::Accept(Visitor *vis)
{
}

ConcreteElementA::ConcreteElementA()
{
}

ConcreteElementA::~ConcreteElementA()
{
}

void ConcreteElementA::Accept(Visitor *vis)
{
    vis->VisitConcreteElementA(this);
    cout << "visiting ConcreteElementA..." << endl;
}
ConcreteElementB::ConcreteElementB()
{
}
ConcreteElementB::~ConcreteElementB()
{
}
void ConcreteElementB::Accept(Visitor *vis)
{
    vis->VisitConcreteElementB(this);
    cout << "visiting ConcreteElementB..." << endl;
}
