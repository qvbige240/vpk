#include <iostream>
#include "component.h"
#include "composite.h"
#include "leaf.h"
using namespace std;

int main()
{
    //不管是叶子Leaf还是Composite对象pRoot、pCom都实现了Operation接口，所以可以一致对待，直接调用Operation()
    //体现了“使得用户对单个对象和组合对象的使用具有一致性”
    Component *lf = new Leaf();
    //执行叶子Operation操作
    lf->Operation();
    Component *lf2 = new Leaf();

    Composite *com = new Composite();
    //组合对象添加叶子节点
    com->Add(lf);
    com->Add(lf2);
    //执行组合对象Operation操作
    com->Operation();

    //获取组合对象中的节点
    Component *ll = com->GetChild(0);
    ll->Operation();

    delete com;
    delete lf;
    delete lf2;
    return 0;
}
