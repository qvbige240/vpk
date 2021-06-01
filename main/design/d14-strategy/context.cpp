
#include <iostream>
#include "strategy.h"
#include "context.h"
using namespace std;

Context::Context(Strategy *stg)
{
    _stg = stg;
}

Context::~Context()
{
    if (_stg)
        delete _stg;
}

void Context::DoAction()
{
    _stg->AlgrithmInterface();
}
