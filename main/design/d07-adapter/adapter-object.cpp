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

class Adapter : public Target
{
public:
    Adapter(Adaptee *ade);
    ~Adapter();
    void Request();

private:
    Adaptee *_ade;
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
Adapter::Adapter(Adaptee *ade)
{
    _ade = ade;
}
Adapter::~Adapter()
{
}
void Adapter::Request()
{
    cout << "---Adapter interface---" << endl;
    _ade->SpecificRequest();
}

int main()
{
    Adaptee *ade = new Adaptee();
    Target *adt = new Adapter(ade);
    adt->Request();

    delete adt;
    delete ade;
    return 0;
}
