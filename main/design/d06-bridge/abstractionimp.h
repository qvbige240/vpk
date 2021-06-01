#ifndef _ABSTRACTIONIMP_H_
#define _ABSTRACTIONIMP_H_

class AbstractionImp
{
public:
    virtual ~AbstractionImp();
    virtual void Operation() = 0;

protected:
    AbstractionImp();
};

class ConcreteAbstractionImpA : public AbstractionImp
{
public:
    ConcreteAbstractionImpA();
    ~ConcreteAbstractionImpA();
    void Operation();
};

class ConcreteAbstractionImpB : public AbstractionImp
{
public:
    ConcreteAbstractionImpB();
    ~ConcreteAbstractionImpB();
    void Operation();
};

#endif
