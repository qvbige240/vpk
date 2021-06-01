
#include <iostream>
#include "mediator.h"
#include "colleague.h"
using namespace std;

int main()
{
    Mediator *cm = new ConcreteMediator();
    Colleague *cA = new ConcreteColleageA(cm);
    Colleague *cB = new ConcreteColleageB(cm);
    cm->IntroColleague(cA, cB);
    cA->SetState("oldA");
    cB->SetState("oldB");
    cA->Action();
    cB->Action();
    cout << endl;

    cA->SetState("newA");
    cA->Action();
    cB->Action();
    cout << endl;

    cB->SetState("old");
    cB->Action();
    cA->Action();

    return 0;
}
