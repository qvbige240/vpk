#include "foo.h"

typedef struct _PrivInfo
{
    pthread_t thread;
    int cond;
} PrivInfo;

FooDemo::FooDemo(void *parent, vmp_callback_func func) : m_parent(parent), m_func(func), m_cond(1), m_priv(NULL)
{
    m_priv = malloc(sizeof(PrivInfo));
}

FooDemo::~FooDemo(void)
{
    printf("~FooDemo %p\n", m_priv);
    if (m_priv)
    {
        printf("free m_priv\n");
        free(m_priv);
        m_priv = NULL;
    }
}

static void *demo_thread(void *arg)
{
    PrivInfo *thiz = (PrivInfo *)arg;

    while (thiz && thiz->cond)
    {
        printf("thread loop\n");
        sleep(5);
    }

    printf("thread cond = %d\n", thiz->cond);
    printf("thread end\n");
    pthread_exit(0);

    return NULL;
}

void task_start(void *ctx)
{
    int ret = 0;
    PrivInfo *thiz = (PrivInfo *)ctx;

    printf("task start\n");
    thiz->cond = 1;
    ret = pthread_create(&thiz->thread, NULL, demo_thread, (void *)thiz);
    if (ret != 0)
        printf("create thread failed\n");
    pthread_detach(thiz->thread);
}

void task_stop(void *ctx)
{
    PrivInfo *thiz = (PrivInfo *)ctx;

    printf("task stop\n");
    thiz->cond = 0;
    // pthread_join(thiz->thread, NULL);
}

void FooDemo::start(void)
{
    printf("start\n");
    task_start(m_priv);
}

void FooDemo::stop(void)
{
    task_stop(m_priv);
    printf("stop\n");
}
