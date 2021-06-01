#include <iostream>
#include "proxy.h"
using namespace std;

Proxy::Proxy()
{
}

Proxy::Proxy(Subject *sub)
{
    _sub = sub;
}

Proxy::~Proxy()
{
    if (_sub)
        delete _sub;
}

void Proxy::Request()
{
    cout << "---Proxy Request..." << endl;
    _sub->Request();
}

Subject::Subject()
{
}

Subject::~Subject()
{
}

ConcreteSubject::ConcreteSubject()
{
}

ConcreteSubject::~ConcreteSubject()
{
}

void ConcreteSubject::Request()
{
    cout << "ConcreteSubject::Request..." << endl;
}


int main()
{
    Subject *sub = new ConcreteSubject();
    Proxy *p = new Proxy(sub);
    p->Request();

    return 0;
}
