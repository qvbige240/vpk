#include <iostream>
#include "element.h"
#include "visitor.h"
using namespace std;

int main()
{
    Visitor *visA = new ConcreteVisitorA();
    Element *elmA = new ConcreteElementA();
    elmA->Accept(visA);

    Visitor *visB = new ConcreteVisitorB();
    Element *elmB = new ConcreteElementB();
    elmB->Accept(visB);

    cout << "-----------" << endl;
    elmA->Accept(visB);
    elmB->Accept(visA);

    delete elmA;
    delete visA;
    delete elmB;
    delete visB;
    return 0;
}
