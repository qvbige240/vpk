#ifndef _SUBJECT_H_
#define _SUBJECT_H_

#include <list>
#include <string>
using namespace std;
class Observer;
class Subject
{
public:
    virtual ~Subject();
    virtual void Attach(Observer *obv);
    virtual void Detach(Observer *obv);
    virtual void Notify();
    virtual void SetState(const string &st) = 0;
    virtual string GetState() = 0;

protected:
    Subject();

private:
    list<Observer *> *_obvs;
};

class ConcreteSubject : public Subject
{
public:
    ConcreteSubject();
    ~ConcreteSubject();
    string GetState();
    void SetState(const string &st);

private:
    string _st;
};

#endif
