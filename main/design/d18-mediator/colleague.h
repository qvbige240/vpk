#ifndef _COLLEAGUE_H_
#define _COLLEAGUE_H_
#include <string>
using namespace std;
class Mediator;
class Colleague
{
public:
    virtual ~Colleague();
    virtual void Action() = 0;
    virtual void SetState(const string &sdt) = 0;
    virtual string GetState() = 0;

protected:
    Colleague();
    Colleague(Mediator *mdt);
    Mediator *_mdt;
};

class ConcreteColleageA : public Colleague
{
public:
    ConcreteColleageA();
    ConcreteColleageA(Mediator *mdt);
    ~ConcreteColleageA();
    void Action();
    void SetState(const string &sdt);
    string GetState();

private:
    string _sdt;
};

class ConcreteColleageB : public Colleague
{
public:
    ConcreteColleageB();
    ConcreteColleageB(Mediator *mdt);
    ~ConcreteColleageB();
    void Action();
    void SetState(const string &sdt);
    string GetState();

private:
    string _sdt;
};

#endif
