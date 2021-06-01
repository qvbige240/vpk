#include <iostream>
#include "state.h"

using namespace std;

State::State()
{
}

State::~State()
{
}

void State::OperationInterface(Context *con)
{
    cout << "State::OperationInterface..." << endl;
}
void State::OperationChangeState(Context *con)
{
}
bool State::ChangeState(Context *con, State *st)
{
    cout << "State::ChangeState" << endl;
    con->ChangeState(st); //State类是Context类的友元类
    return true;
}

ConcreteStateA::ConcreteStateA()
{
}

ConcreteStateA::~ConcreteStateA()
{
}

void ConcreteStateA::OperationInterface(Context *con)
{
    cout << "ConcreteStateA::OperationInterface..." << endl;
}
//执行该状态的行为并改变状态
void ConcreteStateA::OperationChangeState(Context *con)
{
    cout << "+++ConcreteStateA::OperationChangeState..." << endl;
    OperationInterface(con);
    this->ChangeState(con, new ConcreteStateB());
}

ConcreteStateB::ConcreteStateB()
{
}

ConcreteStateB::~ConcreteStateB()
{
}

void ConcreteStateB::OperationInterface(Context *con)
{
    cout << "ConcreteStateB::OperationInterface..." << endl;
}
//执行该状态的行为并改变状态
void ConcreteStateB::OperationChangeState(Context *con)
{
    cout << "+++ConcreteStateB::OperationChangeState..." << endl;
    OperationInterface(con);
    this->ChangeState(con, new ConcreteStateA());
}

/** context **/
Context::Context()
{
    _state = NULL;
}

Context::Context(State *state)
{
    this->_state = state;
}

Context::~Context()
{
    if (_state)
        delete _state;
}

void Context::OperationInterface()
{
    _state->OperationInterface(this);
}

bool Context::ChangeState(State *state)
{
    cout << "Context::ChangeState" << endl;
    if (_state)
    {
        cout << "...delete origin State" << endl;
        delete _state;
        _state = NULL;
    }
    this->_state = state;
    return true;
}
//对请求做处理，并设置下一状态
void Context::OperationChangeState()
{
    _state->OperationChangeState(this);
}

/** main **/
int main()
{
    State *st = new ConcreteStateA();
    Context *con = new Context(st);

    cout << "origin---";
    con->OperationInterface();
    con->OperationChangeState();
    cout << "final---";
    con->OperationInterface();
    cout << "---------------------" << endl;

    con->OperationChangeState();
    cout << "final---";
    con->OperationInterface();

    if (con)
        delete con;
    if (st)
        st = NULL;
    return 0;
}
