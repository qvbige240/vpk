

class Prototype
{
public:
    virtual ~Prototype();
    virtual Prototype *Clone() const = 0;
    int number;

protected:
    Prototype();
};

class ConcretePrototype : public Prototype
{
public:
    ConcretePrototype();
    ConcretePrototype(const ConcretePrototype &cp);
    ~ConcretePrototype();
    Prototype *Clone() const;
};

//
#include <iostream>
using namespace std;

Prototype::Prototype()
{
    cout << "---Prototype constructor" << endl;
    number = 100;
}
Prototype::~Prototype()
{
}
Prototype *Prototype::Clone() const
{
    return 0;
}
ConcretePrototype::ConcretePrototype()
{
    cout << "---ConcretePrototype constructor" << endl;
}
ConcretePrototype::ConcretePrototype(const ConcretePrototype &cp)
{
    cout << "---ConcretePrototype copy..." << endl;
}
ConcretePrototype::~ConcretePrototype()
{
}
Prototype *ConcretePrototype::Clone() const
{
    cout << "---Clone()" << endl;
    return new ConcretePrototype(*this);
}

int main()
{
    Prototype *p_one = new ConcretePrototype();
    cout << "before clone:" << p_one->number << endl;
    Prototype *p_two = p_one->Clone();
    cout << "clone:" << p_two->number << endl;
    p_two->number = 200;

    cout << "----------------------" << endl;
    cout << "origin:" << p_one->number << endl;
    cout << "clone:" << p_two->number << endl;
    delete p_one;
    delete p_two;
    return 0;
}
