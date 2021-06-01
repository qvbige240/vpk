#include <iostream>
#include "memento.h"
using namespace std;

Memento::Memento()
{
}

Memento::~Memento()
{
}

Memento::Memento(const string &sdt)
{
    _sdt = sdt;
}

void Memento::SetState(const string &sdt)
{
    _sdt = sdt;
}

string Memento::GetState()
{
    return _sdt;
}

Originator::Originator()
{
    _sdt = "";
    _mt = NULL;
}

Originator::Originator(const string &sdt)
{
    _sdt = sdt;
    _mt = NULL;
}

Originator::~Originator()
{
}

Memento *Originator::CreateMemento()
{
    return new Memento(_sdt);
}

string Originator::GetState()
{
    return _sdt;
}

void Originator::SetState(const string &sdt)
{
    _sdt = sdt;
}

void Originator::PrintState()
{
    cout << "State:" << this->_sdt << endl;
}
void Originator::SetMemento(Memento *men)
{
}
void Originator::RestoreToMemento(Memento *mt)
{
    this->_sdt = mt->GetState();
}

/** main **/
int main()
{
    Originator *ogn = new Originator();
    //初始化对象，状态为“Old”
    ogn->SetState("old"); //备忘前状态
    ogn->PrintState();
    Memento *mt = ogn->CreateMemento(); //将状态备忘
    cout << "---change state:";
    ogn->SetState("new"); //修改状态
    ogn->PrintState();
    ogn->RestoreToMemento(mt); //恢复修改前状态
    cout << "---restore state:";
    ogn->PrintState();

    return 0;
}
