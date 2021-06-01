#ifndef _MEMENTO_H_
#define _MEMENTO_H_
#include <string>
using namespace std;

class Originator;
class Memento
{
private:
    //将Originatord声明为friend类,可以访问内部信息,但是其他类不能访问
    friend class Originator;
    Memento();
    Memento(const string &sdt);
    ~Memento();
    void SetState(const string &sdt);
    string GetState();

private:
    string _sdt;
};
//负责创建一个备忘录Memento，用以记录当前时刻它的内部状态，并可使用备忘录恢复内部状态
class Originator
{
public:
    Originator();
    Originator(const string &sdt);
    ~Originator();
    Memento *CreateMemento();
    void SetMemento(Memento *men);
    void RestoreToMemento(Memento *mt);
    string GetState();
    void SetState(const string &sdt);
    void PrintState();

private:
    string _sdt;
    Memento *_mt;
};

#endif
