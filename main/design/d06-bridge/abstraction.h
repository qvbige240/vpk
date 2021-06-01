#ifndef _ABSTRACTION_H_
#define _ABSTRACTION_H_

class Abstraction
{
public:
    virtual ~Abstraction();
    virtual void Operation() = 0;

protected:
    Abstraction();
};

class AbstractionImp;
class RefinedAbstraction : public Abstraction
{
public:
    RefinedAbstraction(AbstractionImp *imp);
    ~RefinedAbstraction();
    void Operation();

private:
    AbstractionImp *_imp;
};

#endif
