/**
 * History:
 * ================================================================
 * 2020-01-07 qing.zou created
 *
 */

#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "vpk_sock.h"

/* Resolve IPv4/IPv6 address */
int vpk_getaddrinfo(int af, const char *nodename, unsigned *count, vpk_addrinfo ai[])
{
    char nodecopy[VPK_MAX_HOSTNAME];
    unsigned i;
    int rc;
    struct addrinfo hint, *res, *orig_res;

    // return_val_if_fail(nodename && count && *count && ai, -1);
    // return_val_if_fail(nodename->ptr && nodename->slen, -1);
    // return_val_if_fail(af == AF_INET || af == AF_INET6 || af == AF_UNSPEC, -1);

    // /* Copy node name to null terminated string. */
    // if (nodename->slen >= VPK_MAX_HOSTNAME)
    //     return -1;
    // memcpy(nodecopy, nodename->ptr, nodename->slen);
    // nodecopy[nodename->slen] = '\0';

    return_val_if_fail(nodename && count && *count && ai, -1);
    return_val_if_fail(af == AF_INET || af == AF_INET6 || af == AF_UNSPEC, -1);

    int slen = strlen(nodename);
    if (slen >= VPK_MAX_HOSTNAME)
        return -1;
    memcpy(nodecopy, nodename, slen);
    nodecopy[slen] = '\0';

    /* Call getaddrinfo() */
    memset(&hint, 0x00, sizeof(hint));
    hint.ai_family = af;

    rc = getaddrinfo(nodecopy, NULL, &hint, &res);
    if (rc != 0)
        return -1;

    orig_res = res;

    /* Enumerate each item in the result */
    for (i = 0; i < *count && res; res = res->ai_next)
    {
        /* Ignore unwanted address families */
        if (af != AF_UNSPEC && res->ai_family != af)
            continue;

        /* Store canonical name (possibly truncating the name) */
        if (res->ai_canonname)
        {
            strncpy(ai[i].ai_canonname, res->ai_canonname, sizeof(ai[i].ai_canonname));
            ai[i].ai_canonname[sizeof(ai[i].ai_canonname) - 1] = '\0';
        }
        else
        {
            strcpy(ai[i].ai_canonname, nodecopy);
        }

        /* Store address */
        //PJ_ASSERT_ON_FAIL(res->ai_addrlen <= sizeof(pj_sockaddr), continue);
        if (!res->ai_addrlen <= sizeof(vpk_sockaddr))
            continue;
        memcpy(&ai[i].ai_addr, res->ai_addr, res->ai_addrlen);
        //PJ_SOCKADDR_RESET_LEN(&ai[i].ai_addr);

        /* Next slot */
        ++i;
    }

    *count = i;

    freeaddrinfo(orig_res);

    /* Done */
    return (*count > 0 ? 0 : -1);
}
