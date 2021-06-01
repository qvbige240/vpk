#include <iostream>
#include "invoker.h"
#include "command.h"
using namespace std;

Invoker::Invoker(Command *cmd)
{
    _cmd = cmd;
}

Invoker::~Invoker()
{
}

void Invoker::InvokeFunc()
{
    cout << "Invoker::InvokeFunc" << endl;
    _cmd->Excute();
}
