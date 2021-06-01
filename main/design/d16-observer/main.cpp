
#include <iostream>
#include "subject.h"
#include "observer.h"
using namespace std;

int main()
{
    Subject *sub = new ConcreteSubject();
    //ConcreteObserverA attach list
    Observer *ob1 = new ConcreteObserverA(sub);
    //ConcreteObserverB attach list
    Observer *ob2 = new ConcreteObserverB(sub);

    sub->SetState("old");
    sub->Notify();
    cout << "---------------------------" << endl;
    sub->SetState("new"); //也可以由Observer调用
    Observer *ob3 = new ConcreteObserverA(sub);
    sub->Notify();

    delete ob1;
    delete ob2;
    delete ob3;
    delete sub;
    return 0;
}
