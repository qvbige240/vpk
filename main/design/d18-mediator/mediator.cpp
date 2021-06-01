#include "mediator.h"
#include "colleague.h"

Mediator::Mediator()
{
}

Mediator::~Mediator()
{
}

ConcreteMediator::ConcreteMediator()
{
}

ConcreteMediator::~ConcreteMediator()
{
}

ConcreteMediator::ConcreteMediator(Colleague *clgA, Colleague *clgB)
{
    _clgA = clgA;
    _clgB = clgB;
}

void ConcreteMediator::DoActionFromAtoB()
{
    _clgB->SetState(_clgA->GetState());
}

void ConcreteMediator::SetConcreteColleagueA(Colleague *clgA)
{
    _clgA = clgA;
}

void ConcreteMediator::SetConcreteColleagueB(Colleague *clgB)
{
    _clgB = clgB;
}

Colleague *ConcreteMediator::GetConcreteColleagueA()
{
    return _clgA;
}

Colleague *ConcreteMediator::GetConcreteColleagueB()
{
    return _clgB;
}

void ConcreteMediator::IntroColleague(Colleague *clgA, Colleague *clgB)
{
    _clgA = clgA;
    _clgB = clgB;
}

void ConcreteMediator::DoActionFromBtoA()
{
    _clgA->SetState(_clgB->GetState());
}
