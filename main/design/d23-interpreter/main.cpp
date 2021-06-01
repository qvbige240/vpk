#include <iostream>
#include "context.h"
#include "interpret.h"
using namespace std;

int main()
{
    Context *ct = new Context();
    AbstractExpression *te = new TerminalExpression("hello");
    AbstractExpression *nte = new NonterminalExpression(te, 3);
    nte->Interpret(*ct);

    return 0;
}
