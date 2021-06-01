
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

//Component抽象基类，为组合中的对象声明接口,声明了类共有接口的缺省行为(如这里的Add,Remove,GetChild函数)
class Component
{
public:
    virtual ~Component();

public:
    virtual void Operation() = 0;
    virtual void Add(const Component &);
    virtual void Remove(const Component &);
    virtual Component *GetChild(int);

protected:
    Component();
};

#endif
