/*
 * History:
 * ================================================================
 * 2018-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "vpk.h"

typedef struct pj_str_t
{
    /** Buffer pointer, which is by convention NOT null terminated. */
    char        *ptr;

    /** The length of the string. */
    long        slen;
} pj_str_t;

/*
 * Get hostname.
 */
static const pj_str_t *pj_gethostname(void)
{
    static char buf[128];
    static pj_str_t hostname;

#if 1
    if (hostname.ptr == NULL)
    {
        hostname.ptr = buf;
        if (gethostname(buf, sizeof(buf)) != 0)
        {
            hostname.ptr[0] = '\0';
            hostname.slen = 0;
        }
        else
        {
            hostname.slen = strlen(buf);
            printf("buf[%d]: %s\n", strlen(buf), buf);
        }
    }
#else
    if (gethostname(buf, sizeof(buf)) != 0)
    {
        printf("gethostname error\n");
    }
    else
    {
        printf("buf: %s\n", buf);
    }
#endif
    return &hostname;
}

static void test_getaddrinfo(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usag...\n");
        exit(1);
    }

    struct addrinfo hints;
    struct addrinfo *res, *cur;
    int ret;
    struct sockaddr_in *addr;
    char ipbuf[16];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;   /* Allow IPv4 */
    hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
    hints.ai_protocol = 0;       /* Any protocol */
    hints.ai_socktype = SOCK_STREAM;

    ret = getaddrinfo(argv[1], NULL, &hints, &res);
    //ret = getaddrinfo("zouqing-desktop", NULL, &hints, &res);

    if (ret == -1)
    {
        perror("getaddrinfo");
        exit(1);
    }

    for (cur = res; cur != NULL; cur = cur->ai_next)
    {
        addr = (struct sockaddr_in *)cur->ai_addr;
        printf("%s\n", inet_ntop(AF_INET, &addr->sin_addr, ipbuf, 16));
    }
    freeaddrinfo(res);
    //exit(0);
}


static int main_net(int argc, char *argv[])
{
    pj_gethostname();
    test_getaddrinfo(argc, argv);

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

    main_net(argc, argv);

    return 0;
}
