#include <iostream>
#include "subject.h"
#include "observer.h"
using namespace std;

Subject::Subject()
{
    _obvs = new list<Observer *>;
}

Subject::~Subject()
{
}

void Subject::Attach(Observer *obv)
{
    _obvs->push_front(obv);
}

void Subject::Detach(Observer *obv)
{
    if (obv)
        _obvs->remove(obv);
}

void Subject::Notify()
{
    list<Observer *>::iterator it;
    for (it = _obvs->begin(); it != _obvs->end(); it++)
    {
        (*it)->Update(this);
    }
}

ConcreteSubject::ConcreteSubject()
{
    _st = "";
}

ConcreteSubject::~ConcreteSubject()
{
}

string ConcreteSubject::GetState()
{
    return _st;
}

void ConcreteSubject::SetState(const string &st)
{
    _st = st;
}
