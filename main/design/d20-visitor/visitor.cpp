#include <iostream>
#include "visitor.h"
#include "element.h"
using namespace std;

Visitor::Visitor()
{
}

Visitor::~Visitor()
{
}

ConcreteVisitorA::ConcreteVisitorA()
{
}

ConcreteVisitorA::~ConcreteVisitorA()
{
}

void ConcreteVisitorA::VisitConcreteElementA(Element *elm)
{
    cout << "I will visit ConcreteVisitorA::ConcreteElementA..." << endl;
}
void ConcreteVisitorA::VisitConcreteElementB(Element *elm)
{
    cout << "I will visit ConcreteVisitorA::ConcreteElementB..." << endl;
}
ConcreteVisitorB::ConcreteVisitorB()
{
}
ConcreteVisitorB::~ConcreteVisitorB()
{
}
void ConcreteVisitorB::VisitConcreteElementA(Element *elm)
{
    cout << "I will visit ConcreteVisitorB::ConcreteElementA..." << endl;
}
void ConcreteVisitorB::VisitConcreteElementB(Element *elm)
{
    cout << "I will visit ConcreteVisitorB::ConcreteElementB..." << endl;
}
