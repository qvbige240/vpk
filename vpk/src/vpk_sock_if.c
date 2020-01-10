/**
 * History:
 * ================================================================
 * 2020-01-08 qing.zou created
 *
 */

#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "vpk_sock.h"

#if 0
#include "vpk_logging.h"
#define VPK_TRACE(format, args...) LOG_D(format, ##args)
static const char *get_addr(void *addr)
{
    static char txt[VPK_MAX_ADDR_STRING];
    struct sockaddr *ss = (struct sockaddr *)addr;
    if (ss->sa_family != AF_INET && ss->sa_family != AF_INET6)
        return "?";
    return vpk_addr_to_string2(addr, txt);
}
#else
#define VPK_TRACE(format, args...)
#endif

int vpk_getipinterface(int af, const char *dst, vpk_sockaddr *if_addr, int allow_resolve, vpk_sockaddr *p_dst_addr)
{
    vpk_sockaddr dst_addr;
    unsigned char zero[64];
    int status;

    vpk_sockaddr_init(af, &dst_addr, NULL, 53);
    status = vpk_inet_pton(af, dst, vpk_sockaddr_get_addr(&dst_addr));
    if (status != 0)
    {
        if (allow_resolve)
        {
            status = vpk_sockaddr_init(af, &dst_addr, dst, 53);
        }
        else
        {
            const char *str_dst;
            if (af == AF_INET)
                str_dst = "1.1.1.1";
            else //if (af == AF_INET6)
                str_dst = "1::1";

            status = vpk_sockaddr_init(af, &dst_addr, str_dst, 53);
        }

        if (status != 0)
            return status;
    }

    /* Create UDP socket and connect() to the destination IP */
    vpk_socket_t sock = {0};
    status = vpk_socket_sock(NULL, af, VPK_PROTOTYPE_UDP, &sock);
    if (status != 0)
        return status;

    status = vpk_socket_connect(sock.fd, &dst_addr, get_sockaddr_len(&dst_addr));
    if (status != 0)
    {
        vpk_socket_close(sock.fd);
        return status;
    }

    status = vpk_addr_get_from_sock(sock.fd, if_addr);
    if (status != 0)
    {
        vpk_socket_close(sock.fd);
        return status;
    }

    vpk_socket_close(sock.fd);

    memset(zero, 0x00, sizeof(zero));
    if (memcmp(vpk_sockaddr_get_addr(if_addr), zero, vpk_sockaddr_get_addr_len(if_addr)) == 0)
        return -1;

    if (p_dst_addr)
        *p_dst_addr = dst_addr;

    return 0;
}

int vpk_default_ipinterface(int af, vpk_sockaddr *addr)
{
    const char *ip_dst;
    if (af == AF_INET)
        ip_dst = "1.1.1.1";
    else //if (af == AF_INET6)
        ip_dst = "1::1";

    return vpk_getipinterface(af, ip_dst, addr, 0, NULL);
}

int vpk_enum_if_by_af(int af, unsigned *count, vpk_sockaddr ifs[])
{
    struct ifaddrs *ifap = NULL, *it;
    unsigned max;
    return_val_if_fail(af == AF_INET || af == AF_INET6, -1);

    VPK_TRACE("Starting interface enum with getifaddrs() for af=%d", af);

    if (getifaddrs(&ifap) != 0)
    {
        printf("getifaddrs() failed: %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    it = ifap;
    max = *count;
    *count = 0;

    for (; it != NULL && *count < max; it = it->ifa_next)
    {
        struct sockaddr *ss = it->ifa_addr;
        VPK_TRACE("checking %s", it->ifa_name);
        //printf("checking %s\n", it->ifa_name);
        if ((it->ifa_flags & IFF_UP) == 0)
        {
            VPK_TRACE("  interface is down");
            continue;
        }

#if VPK_SOCK_IGNORE_LOOPBACK_IF
        if (it->ifa_flags & IFF_LOOPBACK)
        {
            VPK_TRACE("  loopback interface");
            continue;
        }
#endif

        if (ss == NULL)
        {
            VPK_TRACE("  NULL address ignored");
            continue;
        }

        if (ss->sa_family != af)
        {
            VPK_TRACE("  address %s ignored (af=%d)", get_addr(ss), ss->sa_family);
            continue;
        }

        if (af == AF_INET && (ntohl(((struct sockaddr_in *)ss)->sin_addr.s_addr) >> 24) == 0)
        {
            VPK_TRACE("  address %s ignored (0.0.0.0/8 class)", get_addr(ss), ss->sa_family);
            continue;
        }

        VPK_TRACE("  address %s (af=%d) added at index %d", get_addr(ss), ss->sa_family, *count);

        memset(&ifs[*count], 0x00, sizeof(ifs[0]));
        memcpy(&ifs[*count], ss, get_sockaddr_len((const vpk_sockaddr *)ss));
        (*count)++;
    }

    freeifaddrs(ifap);

    VPK_TRACE("Done, found %d address(es)", *count);

    return (*count != 0) ? 0 : -1;
}

int vpk_enum_ip_interface(int af, unsigned *count, vpk_sockaddr ifs[])
{
    int status;
    unsigned start = 0;

    if (af == AF_INET6 || af == AF_UNSPEC)
    {
        unsigned max = *count;
        status = vpk_enum_if_by_af(AF_INET6, &max, &ifs[start]);
        if (status == 0)
        {
            start += max;
            (*count) -= max;
        }
    }

    if (af == AF_INET || af == AF_UNSPEC)
    {
        unsigned max = *count;
        status = vpk_enum_if_by_af(AF_INET, &max, &ifs[start]);
        if (status == 0)
        {
            start += max;
            (*count) -= max;
        }
    }

    *count = start;

    return (*count != 0) ? 0 : -1;
}
