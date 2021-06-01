#include <iostream>
#include "receiver.h"
#include "invoker.h"
#include "command.h"
using namespace std;

int main()
{
    //创建具体命令对象cmd并设定它的接收者rev
    Receiver *rev = new Receiver();
    Command *cmd = new ConcreteCommand(rev);
    //请求绑定命令
    Invoker *inv = new Invoker(cmd);
    inv->InvokeFunc();

    delete inv;
    delete cmd;
    delete rev;
    return 0;
}
