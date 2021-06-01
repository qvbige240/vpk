#include <iostream>
#include "command.h"
#include "receiver.h"
using namespace std;

Command::Command()
{
}

Command::~Command()
{
}

ConcreteCommand::ConcreteCommand(Receiver *rev)
{
    _rev = rev;
}

ConcreteCommand::~ConcreteCommand()
{
}

void ConcreteCommand::Excute()
{
    cout << "ConcreteCommand::Excute" << endl;
    _rev->Action();
}
