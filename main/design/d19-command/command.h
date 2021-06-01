
#ifndef _COMMAND_H_
#define _COMMAND_H_

class Receiver;
class Command
{
public:
    virtual ~Command();
    virtual void Excute() = 0;

protected:
    Command();
};
class ConcreteCommand : public Command
{
public:
    ConcreteCommand(Receiver *rev);
    ~ConcreteCommand();
    void Excute();

private:
    Receiver *_rev;
};

#endif
