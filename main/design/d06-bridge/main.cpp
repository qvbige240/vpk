#include <iostream>
#include "abstraction.h"
#include "abstractionimp.h"
using namespace std;

int main()
{
    // Bridge模式将抽象和实现分别独立实现
    // 实现部分ConcreteAbstractionImplementA
    AbstractionImp *imp = new ConcreteAbstractionImpA();
    // 抽象部分RefinedAbstractionA
    Abstraction *abs = new RefinedAbstraction(imp);
    abs->Operation();

    cout << "-----------------------------------------" << endl;
    AbstractionImp *imp2 = new ConcreteAbstractionImpB();
    Abstraction *abs2 = new RefinedAbstraction(imp2);
    abs2->Operation();

    delete abs;
    delete imp;
    delete abs2;
    delete imp2;
    return 0;
}
