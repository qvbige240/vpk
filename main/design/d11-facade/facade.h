#ifndef _FACADE_H_
#define _FACADE_H_

class Subsystem1;
class Subsystem2;
class Facade
{
public:
    static Facade *GetInstance();
    static void CloseInstance();
    void OperationWrapper();

protected:
    Facade();
    ~Facade();

private:
    Subsystem1 *_subs1;
    Subsystem2 *_subs2;
    static Facade *_instance;
};

class Subsystem1
{
public:
    Subsystem1();
    ~Subsystem1();
    void Operation();
};

class Subsystem2
{
public:
    Subsystem2();
    ~Subsystem2();
    void Operation();
};

#endif
