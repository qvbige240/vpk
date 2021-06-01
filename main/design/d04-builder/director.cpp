#include "director.h"
#include "builder.h"
#include "product.h"

Director::Director()
{
    _bld = NULL;
}

Director::~Director()
{
    if (_bld)
    {
        delete _bld;
        _bld = NULL;
    }
}

ProductFood *Director::foodInfoPoint(char ch)
{
    if (_bld)
    {
        delete _bld;
        _bld = NULL;
    }
    if (ch == 'A')
    {
        _bld = new ConcreteBuilder1();
    }
    else if (ch == 'B')
    {
        _bld = new ConcreteBuilder2();
    }
    else
    { //other
    }
    _bld->BuildCoolDish("CoolDish");
    _bld->BuildDrink("Drink");
    _bld->BuildRice("Rice");
    return _bld->GetProductPoint();
}
