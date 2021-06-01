
#ifndef _PRODUCT_H_
#define _PRODUCT_H_

class Product
{
public:
    virtual ~Product() = 0;

protected:
    Product();
};

class ConcreteProduct1 : public Product
{
public:
    ~ConcreteProduct1();
    ConcreteProduct1();
};
class ConcreteProduct3 : public Product
{
public:
    ~ConcreteProduct3();
    ConcreteProduct3();
};

#endif
