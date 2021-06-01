#include <iostream>
#include "strategy.h"
#include "context.h"
using namespace std;

int main()
{
    /**
     * Strategy模式和Template模式实际是实现一个抽象接口的两种方式：继承和组合之间的区别。
     * 要实现一个抽象接口:
     *  Template模式的继承是一种方式：我们将抽象接口声明在基类中，将具体的实现放在具体子类中。
     *  Strategy模式的组合（委托）是另外一种方式：我们将接口的实现放在被组合对象中，将抽象接口放在组合类中。
     *  这两种方式各有优缺点
     */
    //策略A与B可替换
    Strategy *psA = new ConcreteStrategyA();
    Context *pcA = new Context(psA);
    pcA->DoAction();
    if (pcA)
        delete pcA;

    Strategy *psB = new ConcreteStrategyB();
    Context *pcB = new Context(psB);
    pcB->DoAction();
    if (pcB)
        delete pcB;
    return 0;
}
