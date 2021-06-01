
#ifndef _OBSERVER_H_
#define _OBSERVER_H_
#include <string>
#include "subject.h"
using namespace std;

class Observer
{
public:
    virtual ~Observer();
    virtual void Update(Subject *sub) = 0;
    virtual void PrintInfo() = 0;

protected:
    Observer();
    string _st;
};

class ConcreteObserverA : public Observer
{
public:
    //传入Subject作为参数,这样可以让一个View属于多个的Subject
    ConcreteObserverA(Subject *sub);
    void Update(Subject *sub);
    virtual Subject *GetSubject();
    virtual ~ConcreteObserverA();
    void PrintInfo();

private:
    Subject *_sub;
};

class ConcreteObserverB : public Observer
{
public:
    //传入Subject作为参数,这样可以让一个View属于多个的Subject
    ConcreteObserverB(Subject *sub);
    void Update(Subject *sub);
    virtual Subject *GetSubject();
    virtual ~ConcreteObserverB();
    void PrintInfo();

private:
    Subject *_sub;
};

#endif
