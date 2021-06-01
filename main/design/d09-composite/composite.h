#ifndef _COMPOSITE_H_
#define _COMPOSITE_H_

#include <vector>
#include <iostream>
#include "component.h"
using namespace std;

//Composite：含有子组件的类
class Composite : public Component
{
public:
    Composite();
    ~Composite();

public:
    //实现的接口
    void Operation();
    void Add(Component *com);
    void Remove(Component *com);
    Component *GetChild(int index);

private:
    //这里采用vector来保存子组件
    vector<Component *> comVec;
};

#endif
