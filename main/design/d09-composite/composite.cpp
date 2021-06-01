#include "component.h"
#include "composite.h"

Composite::Composite()
{
}

Composite::~Composite()
{
}

void Composite::Operation()
{
    cout << "Composite Operation the vector" << endl;
    vector<Component *>::iterator comIter = comVec.begin();
    for (; comIter != comVec.end(); comIter++)
    {
        (*comIter)->Operation();
    }
}

void Composite::Add(Component *com)
{
    cout << "Composite::Add..." << endl;
    comVec.push_back(com);
}
void Composite::Remove(Component *com)
{
}
Component *Composite::GetChild(int index)
{
    cout << "Composite::GetChild..." << endl;

    if (index < 0 || index >= comVec.size())
    {
        return NULL;
    }

    return comVec[index];
}
