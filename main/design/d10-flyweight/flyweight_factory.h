#ifndef _FLYWEIGHTFACTORY_H_
#define _FLYWEIGHTFACTORY_H_

#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include "flyweight.h"
using namespace std;

class FlyweightFactory
{
public:
    FlyweightFactory();
    ~FlyweightFactory();
    //获得一个请求的Flyweight对象
    Flyweight *GetFlyweight(const string &key);

private:
    //保存内部状态对象的容器
    vector<Flyweight *> _fly;
};

#endif
