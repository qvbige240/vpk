#ifndef FOO_H
#define FOO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef int (*vmp_callback_func)(void *p, int msg, void *arg);

class FooDemo
{
public:
    FooDemo(void *parent, vmp_callback_func func);
    ~FooDemo(void);

public:
    void start(void);
    void stop(void);

private:
    int m_cond;

    void *m_parent;
    void *m_priv;

    vmp_callback_func m_func;
};

#endif // FOO_H
