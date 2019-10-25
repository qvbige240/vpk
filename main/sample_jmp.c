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

#include "vpk.h"

jmp_buf buf;

static void foo_call()
{
    printf("foo_call() longjmp \n");
    longjmp(buf, 10);
    printf("after foo_call() longjmp \n");
}
static int setjmp_test()
{
    int ret = setjmp(buf);
    if (ret)
        printf("back from foo_call(), ret = %d\n", ret);
    else
    {
        printf("before foo_call(), ret = %d\n", ret);
        foo_call();
    }

    return 0;
}

static int main_jump(int argc, char *argv[])
{
    setjmp_test();
    return 0;
}

#ifdef USE_ZLOG
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
