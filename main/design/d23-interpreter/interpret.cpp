#include <iostream>
#include "interpret.h"
using namespace std;

AbstractExpression::AbstractExpression()
{
}

AbstractExpression::~AbstractExpression()
{
}

void AbstractExpression::Interpret(const Context &ct)
{
}

TerminalExpression::TerminalExpression(const string &statement)
{
    _statement = statement;
}

TerminalExpression::~TerminalExpression()
{
}

void TerminalExpression::Interpret(const Context &ct)
{
    cout << "TerminalExpression::Interpret---" << this->_statement << endl;
}
NonterminalExpression::NonterminalExpression(AbstractExpression *expression, int times)
{
    this->_expression = expression;
    this->_times = times;
}

NonterminalExpression::~NonterminalExpression()
{
}

void NonterminalExpression::Interpret(const Context &ct)
{
    for (int i = 0; i < _times; i++)
    {
        _expression->Interpret(ct);
    }
}
