#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

//抽象模板，并定义了一个模板方法
class AbstractClass
{
public:
    virtual ~AbstractClass();
    //具体的模板方法，给出了逻辑的骨架，而逻辑的组成是一些相应的抽象操作，它们都推迟到子类中实现
    void TemplateMethod();

protected:
    //一些抽象行为，放到子类中去实现
    virtual void PrimitiveOperation1() = 0;
    virtual void PrimitiveOperation2() = 0;
    AbstractClass();
};

//实现基类所定义的抽象方法
class ConcreteClass1 : public AbstractClass
{
public:
    ConcreteClass1();
    ~ConcreteClass1();

protected:
    //实现基类定义的抽象行为
    void PrimitiveOperation1();
    void PrimitiveOperation2();
};
//实现基类所定义的抽象方法
class ConcreteClass2 : public AbstractClass
{
public:
    ConcreteClass2();
    ~ConcreteClass2();

protected:
    //实现基类定义的抽象行为
    void PrimitiveOperation1();
    void PrimitiveOperation2();
};

#endif
