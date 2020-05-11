
/*
 * History:
 * ================================================================
 * 2020-05-08 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#include <assert.h>

#include "vpk.h"

#define VPK_MCAST_TEST_PORT      15353
#define VPK_IPV4_MCAST_GROUP "224.0.0.251"
#define VPK_IPV6_MCAST_GROUP "ff02::fb"

static void mcast_group_ipv4(struct sockaddr_in *ret_sa) {
    assert(ret_sa);

    memset(ret_sa, 0, sizeof(struct sockaddr_in));
    ret_sa->sin_family = AF_INET;
    ret_sa->sin_port = htons(VPK_MCAST_TEST_PORT);
    inet_pton(AF_INET, VPK_IPV4_MCAST_GROUP, &ret_sa->sin_addr);
}

static int reuseaddr(int fd)
{
    int yes;

    yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        LOG_W("SO_REUSEADDR failed: %s", strerror(errno));
        return -1;
    }

#ifdef SO_REUSEPORT
    yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0)
    {
        LOG_W("SO_REUSEPORT failed: %s", strerror(errno));
        if (errno != ENOPROTOOPT)
            return -1;
    }
#endif

    return 0;
}

static int bind_with_warn(int fd, const struct sockaddr *sa, socklen_t l)
{
    assert(fd >= 0);
    assert(sa);
    assert(l > 0);

    if (bind(fd, sa, l) < 0)
    {

        if (errno != EADDRINUSE)
        {
            LOG_W("bind() failed: %s", strerror(errno));
            return -1;
        }

        LOG_W("*** WARNING: Detected another %s mDNS stack running on this host. This makes mDNS unreliable and is thus not recommended. ***",
              sa->sa_family == AF_INET ? "IPv4" : "IPv6");

        /* Try again, this time with SO_REUSEADDR set */
        if (reuseaddr(fd) < 0)
            return -1;

        if (bind(fd, sa, l) < 0)
        {
            LOG_W("bind() failed: %s", strerror(errno));
            return -1;
        }
    }
    else
    {

        /* We enable SO_REUSEADDR afterwards, to make sure that the
         * user may run other mDNS implementations if he really
         * wants. */

        if (reuseaddr(fd) < 0)
            return -1;
    }

    return 0;
}

int avahi_open_socket_ipv4(int no_reuse)
{
    struct sockaddr_in local;
    int fd = -1, r, ittl;
    uint8_t ttl, cyes;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        LOG_W("socket() failed: %s", strerror(errno));
        goto fail;
    }

    ttl = 255;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        LOG_W("IP_MULTICAST_TTL failed: %s", strerror(errno));
        goto fail;
    }

    ittl = 255;
    if (setsockopt(fd, IPPROTO_IP, IP_TTL, &ittl, sizeof(ittl)) < 0)
    {
        LOG_W("IP_TTL failed: %s", strerror(errno));
        goto fail;
    }

    cyes = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &cyes, sizeof(cyes)) < 0)
    {
        LOG_W("IP_MULTICAST_LOOP failed: %s", strerror(errno));
        goto fail;
    }

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(VPK_MCAST_TEST_PORT);

    if (no_reuse)
        r = bind(fd, (struct sockaddr *)&local, sizeof(local));
    else
        r = bind_with_warn(fd, (struct sockaddr *)&local, sizeof(local));

    if (r < 0)
        goto fail;

    // if (ipv4_pktinfo (fd) < 0)
    //      goto fail;

    // if (avahi_set_cloexec(fd) < 0)
    // {
    //     LOG_W("FD_CLOEXEC failed: %s", strerror(errno));
    //     goto fail;
    // }

    // if (avahi_set_nonblock(fd) < 0)
    // {
    //     LOG_W("O_NONBLOCK failed: %s", strerror(errno));
    //     goto fail;
    // }

    return fd;

fail:
    if (fd >= 0)
        close(fd);

    return -1;
}

int mcast_join_ipv4(int fd, int idx, int join)
{
#ifdef HAVE_STRUCT_IP_MREQN
    struct ip_mreqn mreq;
#else
    struct ip_mreq mreq;
#endif
    struct sockaddr_in sa;

    assert(fd >= 0);
    assert(idx >= 0);
    //assert(a);

    memset(&mreq, 0, sizeof(mreq));
#ifdef HAVE_STRUCT_IP_MREQN
    mreq.imr_ifindex = idx;
    mreq.imr_address.s_addr = htonl(INADDR_ANY);
#else
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
#endif
    mcast_group_ipv4(&sa);
    mreq.imr_multiaddr = sa.sin_addr;

    /* Some network drivers have issues with dropping membership of
     * mcast groups when the iface is down, but don't allow rejoining
     * when it comes back up. This is an ugly workaround */
    if (join)
        setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));

    if (setsockopt(fd, IPPROTO_IP, join ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        LOG_W("%s failed: %s", join ? "IP_ADD_MEMBERSHIP" : "IP_DROP_MEMBERSHIP", strerror(errno));
        return -1;
    }

    return 0;
}

int mcast_client()
{
    int fd;
    struct sockaddr_in mcast_addr;

    fd = avahi_open_socket_ipv4(0);

    // if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    // {
    //     LOG_W("socket() failed: %s", strerror(errno));
    //     return -1;
    // }

    mcast_group_ipv4(&mcast_addr);

#define TEST_DATA "broadcast hello world!"

    char ipbuf[16];
    while (1)
    {
        int n = sendto(fd, TEST_DATA, sizeof(TEST_DATA), 0, (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
        if (n < 0)
        {
            LOG_W("sendto failed: %s", strerror(errno));
            return -1;
        }
        LOG_D(" sendto %s", inet_ntop(AF_INET, &mcast_addr.sin_addr, ipbuf, 16));

        sleep(5);
    }

    return 0;
}

int mcast_server()
{
    int ret;
    int fd;
    struct sockaddr_in local_addr;

    fd = avahi_open_socket_ipv4(0);
    mcast_join_ipv4(fd, 1, 1);

    int i = 0;
    char ipbuf[16];
    char buff[1024];
    for (i = 0; i < 5; i++)
    {
        int addr_len = 0;
        addr_len = sizeof(local_addr);
        memset(buff, 0, 1024);

        ret = recvfrom(fd, buff, 1024, 0, (struct sockaddr *)&local_addr, (socklen_t *)&addr_len);
        if (ret < 0)
        {
            LOG_W("recvfrom failed: %s", strerror(errno));
        }

        LOG_D(" recv %dst message from %s: %s", i, inet_ntop(AF_INET, &local_addr.sin_addr, ipbuf, 16), buff);

        sleep(5);
    }

    return 0;
}

static int main_mcast(int argc, char *argv[])
{
    char* type = "client";
	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 't':
				type = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s", type);

	// double elapsed;
	// struct timeval result, prev, next;	

	// gettimeofday(&prev, 0);

	if (strcasecmp(type, "server") == 0) {
		mcast_server();
	} else if (strcasecmp(type, "client") == 0) {
		mcast_client();
	} else {
		mcast_client();
	}

    printf("\n");

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

// ./mcast -t server
int main(int argc, char *argv[])
{
    //int ret = 0;

#ifdef USE_ZLOG
    sample_zlog_init(0);
#endif // USE_ZLOG

    main_mcast(argc, argv);

    return 0;
}
