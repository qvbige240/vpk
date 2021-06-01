
#ifndef _FACTORY_H_
#define _FACTORY_H_

class Product;
class Factory
{
public:
    virtual ~Factory() = 0;
    virtual Product *CreateProduct() = 0;
    void setFactoryMethod(int flag);

protected:
    Factory();
    int _flag;
};

class ConcreteFactory : public Factory
{
public:
    ~ConcreteFactory();
    ConcreteFactory();
    virtual Product *CreateProduct();
};

#endif
