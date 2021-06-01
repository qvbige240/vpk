#include <iostream>
#include "aggregate.h"
#include "iterator.h"
using namespace std;

int main()
{
    Aggregate *ag = new ConcreteAggregate();
    Iterator *it = new ConcreteIterator(ag);
    for (; !(it->IsDone()); it->Next())
    {
        cout << "---CurrentItem:" << it->CurrentItem() << endl;
    }

    delete it;
    delete ag;
    return 0;
}
