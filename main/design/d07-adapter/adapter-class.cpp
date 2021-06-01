class Target
{
public:
    Target();
    virtual ~Target();
    virtual void Request();
};

class Adaptee
{
public:
    Adaptee();
    ~Adaptee();
    void SpecificRequest();
};

class Adapter : public Target, public Adaptee
{
public:
    Adapter();
    ~Adapter();
    void Request();
};

//
#include <iostream>
using namespace std;

Target::Target()
{
}

Target::~Target()
{
}

void Target::Request()
{
    cout << "Target::Request" << endl;
}
Adaptee::Adaptee()
{
}
Adaptee::~Adaptee()
{
}
void Adaptee::SpecificRequest()
{
    cout << "Adaptee::SpecificRequest" << endl;
}
Adapter::Adapter()
{
}
Adapter::~Adapter()
{
}
void Adapter::Request()
{
    cout << "---Adapter interface---" << endl;
    this->SpecificRequest();
}

int main()
{
    Target *adt = new Adapter();
    adt->Request();

    delete adt;
    adt = NULL;
    return 0;
}
