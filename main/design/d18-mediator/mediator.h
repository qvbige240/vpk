#ifndef _MEDIATOR_H_
#define _MEDIATOR_H_

class Colleague;
class Mediator
{
public:
    virtual ~Mediator();
    virtual void IntroColleague(Colleague *clgA, Colleague *clgB) = 0;
    virtual void DoActionFromAtoB() = 0;
    virtual void DoActionFromBtoA() = 0;

protected:
    Mediator();
};

class ConcreteMediator : public Mediator
{
public:
    ConcreteMediator();
    ~ConcreteMediator();
    ConcreteMediator(Colleague *clgA, Colleague *clgB);
    void SetConcreteColleagueA(Colleague *clgA);
    void SetConcreteColleagueB(Colleague *clgB);
    Colleague *GetConcreteColleagueA();
    Colleague *GetConcreteColleagueB();
    void IntroColleague(Colleague *clgA, Colleague *clgB);
    void DoActionFromAtoB();
    void DoActionFromBtoA();

private:
    Colleague *_clgA;
    Colleague *_clgB;
};

#endif
