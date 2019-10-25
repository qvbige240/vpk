/*
 * History:
 * ================================================================
 * 2019-10-25 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>


#if 1
int max_iteration;

jmp_buf Main;
jmp_buf PointPing;
jmp_buf PointPong;

void Ping(void);
void Pong(void);

void setjmp_test2(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Use %s max-#-of-lines\n", argv[0]);
        exit(1);
    }
    max_iteration = abs(atoi(argv[1]));
    if (setjmp(Main) == 0)
        Ping();
    if (setjmp(Main) == 0)
        Pong();
    longjmp(PointPing, 1);
}

void Ping(void)
{
    int i;

    if (setjmp(PointPing) == 0)
    {
        i = 1;
        longjmp(Main, 1);
    }
    while (1)
    {
        printf("Ping (%2d) - ", i);
        i++;
        if (setjmp(PointPing) == 0)
            longjmp(PointPong, 1);
    }
}

void Pong(void)
{
    int j;

    if (setjmp(PointPong) == 0)
    {
        j = 1;
        longjmp(Main, 1);
    }
    while (1)
    {
        printf("Pong (%2d)\n", j);
        j++;
        if (j > max_iteration)
            exit(0);
        if (setjmp(PointPong) == 0)
            longjmp(PointPing, 1);
    }
}
#else
int max_iteration, iter;
jmp_buf Main, PointA, PointB;

void Ping(void);
void Pong(void);

void setjmp_test2(int argc, char *argv[])
{
    max_iteration = abs(atoi(argv[1]));
    iter = 1;
    if (setjmp(Main) == 0)
        Ping();
    if (setjmp(Main) == 0)
        Pong();
    longjmp(PointA, 1);
}

void Ping(void)
{
    if (setjmp(PointA) == 0)
        longjmp(Main, 1);
    while (1)
    {
        printf("%3d : Ping-", iter);
        if (setjmp(PointA) == 0)
            longjmp(PointB, 1);
    }
}

void Pong(void)
{
    if (setjmp(PointB) == 0)
        longjmp(Main, 1);
    while (1)
    {
        printf("Pong\n");
        iter++;
        if (iter > max_iteration)
            exit(0);
        if (setjmp(PointB) == 0)
            longjmp(PointA, 1);
    }
}
#endif

jmp_buf buf;

static void foo_call()
{
    printf("foo_call() longjmp \n");
    longjmp(buf, 10);
    printf("after foo_call() longjmp \n");
}
static int setjmp_test1()
{
    int ret = setjmp(buf);
    if (ret)
        printf("back from foo_call(), ret = %d\n", ret);
    else
    {
        printf("before foo_call(), ret = %d\n", ret);
        foo_call();
    }

    // sleep(1);
    // printf("re-run\n");
    // ret = setjmp(buf);
    // if (ret)
    //     printf("back from foo_call(), ret = %d\n", ret);
    // else
    // {
    //     printf("before foo_call(), ret = %d\n", ret);
    //     foo_call();
    // }

    sleep(1);
    longjmp(buf, 9);

    return 0;
}

/* gcc -g -O0 -o aaa sample_jmp.c */
static int main_jump(int argc, char *argv[])
{
    setjmp_test1();
    printf("\n");
    setjmp_test2(argc, argv);
    printf("\n");

    return 0;
}

#ifdef USE_ZLOG
#include "vpk.h"
#define SAMPLE_ZLOG_FILE_PATH "."
#define SAMPLE_ZLOG_CONF_FILE "./zlog.conf"
int sample_zlog_init(int procname)
{
    int rc;

    if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH))
    {
        int ret = 0;
        char tmp[256] = {0};
        vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
        printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
        ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
        printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
    }

    rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
    if (rc)
    {
        printf("zlog init failed\n");
        return -1;
    }

    LOG_D("hello, zlog");

    return 0;
}
#endif

int main(int argc, char *argv[])
{
    //int ret = 0;

#ifdef USE_ZLOG
    sample_zlog_init(0);
#endif // USE_ZLOG

    main_jump(argc, argv);

    return 0;
}
