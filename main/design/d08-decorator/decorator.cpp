
//Component抽象类，定义该类对象的接口
class Component
{
public:
    virtual ~Component();
    virtual void Operation() = 0;

protected:
    Component();
};

class ConcreteComponent : public Component
{
public:
    ConcreteComponent();
    ~ConcreteComponent();
    void Operation();
};

//Decorator：装饰抽象类，继承自Component
class Decorator : public Component
{
public:
    Decorator(Component *com);
    virtual ~Decorator();
    void Operation();

protected:
    Component *_com;
};

//ConcreteDecorator就是具体的装饰对象之一，起到给Component添加职责的功能
class ConcreteDecorator : public Decorator
{
public:
    ConcreteDecorator(Component *com);
    ~ConcreteDecorator();
    void Operation();
    void AddedBehavior();
};

//ConcreteDecorator就是具体的装饰对象之二，起到给Component再次添加职责的功能
class ConcreteDecoratorB : public Decorator
{
public:
    ConcreteDecoratorB(Component *com);
    ~ConcreteDecoratorB();
    void Operation();
    void AddedBehaviorB();
};

//
#include <iostream>
using namespace std;

Component::Component()
{
}

Component::~Component()
{
}

void Component::Operation()
{
}

ConcreteComponent::ConcreteComponent()
{
}

ConcreteComponent::~ConcreteComponent()
{
}

void ConcreteComponent::Operation()
{
    cout << "---origin:ConcreteComponent::Operation..." << endl;
}
Decorator::Decorator(Component *com)
{
    this->_com = com;
}

Decorator::~Decorator()
{
}

void Decorator::Operation()
{
}

ConcreteDecorator::ConcreteDecorator(Component *com) : Decorator(com)
{
}

ConcreteDecorator::~ConcreteDecorator()
{
}

void ConcreteDecorator::Operation()
{
    cout << "ConcreteDecorator::Operation..." << endl;
    _com->Operation();
    this->AddedBehavior();
}

void ConcreteDecorator::AddedBehavior()
{
    cout << "+++addition:ConcreteDecorator::AddedBehavior..." << endl;
}
ConcreteDecoratorB::ConcreteDecoratorB(Component *com)
    : Decorator(com)
{
}
ConcreteDecoratorB::~ConcreteDecoratorB()
{
}
void ConcreteDecoratorB::Operation()
{
    cout << "ConcreteDecoratorB::Operation..." << endl;
    _com->Operation();
    this->AddedBehaviorB();
}

void ConcreteDecoratorB::AddedBehaviorB()
{
    cout << "+++additionB:ConcreteDecorator::AddedBehaviorB..." << endl;
}

int main()
{
    Component *com = new ConcreteComponent();
    Decorator *dec = new ConcreteDecorator(com);
    dec->Operation();

    Decorator *decB = new ConcreteDecoratorB(com);
    decB->Operation();

    delete dec;
    delete decB;
    if (com)
        delete com;
    return 0;
}
