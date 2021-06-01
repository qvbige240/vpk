#include <iostream>
#include "flyweight.h"
#include "flyweight_factory.h"
using namespace std;

int main()
{
    //外部状态extrinsicState
    string extrinsicState = "ext";
    FlyweightFactory *fc = new FlyweightFactory();
    Flyweight *fw1 = fc->GetFlyweight("hello");
    //应用外部状态
    fw1->Operation(extrinsicState);

    Flyweight *fw2 = fc->GetFlyweight("world!");
    Flyweight *fw3 = fc->GetFlyweight("hello");

    delete fc;
    fc = NULL;
    return 0;
}
