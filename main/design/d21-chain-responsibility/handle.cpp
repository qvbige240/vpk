#include <iostream>
#include "handle.h"
using namespace std;

Handle::Handle()
{
    _succ = NULL;
}
Handle::~Handle()
{
    if (_succ)
    {
        delete _succ;
        _succ = NULL;
    }
}
Handle::Handle(Handle *succ)
{
    _succ = succ;
}
void Handle::HandleRequest()
{
    cout << "Handle->HandleRequest()" << endl;
    // _succ->HandleRequest();
}
void Handle::SetSuccessor(Handle *succ)
{
    _succ = succ;
}
Handle *Handle::GetSuccessor()
{
    return _succ;
}

ConcreteHandleA::ConcreteHandleA()
{
}
ConcreteHandleA::~ConcreteHandleA()
{
}
ConcreteHandleA::ConcreteHandleA(Handle *succ) : Handle(succ)
{
    // _succ = succ;
}
void ConcreteHandleA::HandleRequest()
{
    cout << "ConcreteHandleA->HandleRequest()" << endl;
    // this->HandleRequest();
}

ConcreteHandleB::ConcreteHandleB()
{
}
ConcreteHandleB::~ConcreteHandleB()
{
}
ConcreteHandleB::ConcreteHandleB(Handle *succ) : Handle(succ)
{
    // _succ = succ;
}
void ConcreteHandleB::HandleRequest()
{
    cout << "ConcreteHandleB->HandleRequest()" << endl;
    // this->GetSuccessor()->HandleRequest();
}

/** main **/
int main()
{
    Handle *h1 = new ConcreteHandleA();
    Handle *h2 = new ConcreteHandleB();
    h1->HandleRequest();
    cout << "--------------" << endl;
    h1->SetSuccessor(h2);
    h1->HandleRequest();

    delete h1;
    return 0;
}
