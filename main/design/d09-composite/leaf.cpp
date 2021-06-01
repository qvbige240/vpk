#include <iostream>
#include "leaf.h"
using namespace std;

Leaf::Leaf()
{
    cout << "---Leaf constructor" << endl;
}
Leaf::~Leaf()
{
}
void Leaf::Operation()
{
    cout << "Leaf::Operation..." << endl;
}
