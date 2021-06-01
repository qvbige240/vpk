#ifndef _FLYWEIGHT_H_
#define _FLYWEIGHT_H_
#include <string>
using namespace std;

class Flyweight
{
public:
    virtual ~Flyweight();
    //操作外部状态extrinsicState
    virtual void Operation(const string &extrinsicState) = 0;
    string GetIntrinsicState();

protected:
    Flyweight(string intrinsicState);

private:
    string _intrinsicState;
};

class ConcreteFlyweight : public Flyweight
{
public:
    ConcreteFlyweight(string intrinsicState);
    ~ConcreteFlyweight();
    //实现接口函数
    void Operation(const string &extrinsicState);
};

class UnsharedConcreteFlyweight : public Flyweight
{
public:
    virtual void Operation(const string &extrinsicState);
    UnsharedConcreteFlyweight(string intrinsicState);
    ~UnsharedConcreteFlyweight();
};
#endif
