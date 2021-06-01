#ifndef _AGGREGATE_H_
#define _AGGREGATE_H_

class Iterator;
class Aggregate
{
public:
    virtual ~Aggregate();
    virtual Iterator *CreateIterator() = 0;
    virtual int GetItem(int idx) = 0;
    virtual int GetSize() = 0;

protected:
    Aggregate();
};

class ConcreteAggregate : public Aggregate
{
public:
    enum
    {
        SIZE = 3
    };
    ConcreteAggregate();
    ~ConcreteAggregate();
    Iterator *CreateIterator();
    int GetItem(int idx);
    int GetSize();

private:
    int _objs[SIZE];
};

#endif
