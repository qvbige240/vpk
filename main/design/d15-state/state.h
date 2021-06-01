#ifndef _STATE_H_
#define _STATE_H_

class Context;
class State
{
public:
    State();
    virtual ~State();
    virtual void OperationInterface(Context *) = 0;
    virtual void OperationChangeState(Context *) = 0;

protected:
    bool ChangeState(Context *con, State *st);
};

class ConcreteStateA : public State
{
public:
    ConcreteStateA();
    virtual ~ConcreteStateA();
    virtual void OperationInterface(Context *);
    virtual void OperationChangeState(Context *);
};

class ConcreteStateB : public State
{
public:
    ConcreteStateB();
    virtual ~ConcreteStateB();
    virtual void OperationInterface(Context *);
    virtual void OperationChangeState(Context *);
};

/** context **/
class Context
{
public:
    Context();
    Context(State *state);
    ~Context();
    void OperationInterface();
    void OperationChangeState();

private:
    bool ChangeState(State *state);
    friend class State; //表明在State类中可以访问Context类的private字段
    State *_state;
};

#endif
