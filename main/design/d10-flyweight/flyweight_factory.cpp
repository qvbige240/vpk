#include "flyweight_factory.h"

FlyweightFactory::FlyweightFactory()
{
}

FlyweightFactory::~FlyweightFactory()
{
    vector<Flyweight *>::iterator it = _fly.begin();
    for (; it != _fly.end(); it++)
    {
        delete *it;
        *it = NULL;
    }
}

Flyweight *FlyweightFactory::GetFlyweight(const string &key)
{
    vector<Flyweight *>::iterator it = _fly.begin();
    for (; it != _fly.end(); it++)
    {
        if ((*it)->GetIntrinsicState() == key)
        {
            cout << "---already created by users..." << key << endl;
            return *it;
        }
    }
    Flyweight *fn = new ConcreteFlyweight(key);
    _fly.push_back(fn);
    return fn;
}
