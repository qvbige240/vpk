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

char *vpk_gethostname(char *hostname)
{
    char buf[VPK_MAX_HOSTNAME];

    if (gethostname(buf, sizeof(buf)) != 0)
    {
        printf("gethostname error\n");
        return NULL;
    }

    strncpy(hostname, buf, sizeof(buf));

    return hostname;
}

/* Resolve the IP address of local machine */
int vpk_gethostip(int af, vpk_sockaddr *addr)
{
    unsigned i, count, cand_cnt;
    enum
    {
        CAND_CNT = 8,

        /* Weighting to be applied to found addresses */
        WEIGHT_HOSTNAME = 1, /* hostname IP is not always valid! */
        WEIGHT_DEF_ROUTE = 2,
        WEIGHT_INTERFACE = 1,
        WEIGHT_LOOPBACK = -5,
        WEIGHT_LINK_LOCAL = -4,
        WEIGHT_DISABLED = -50,

        MIN_WEIGHT = WEIGHT_DISABLED + 1 /* minimum weight to use */
    };
    /* candidates: */
    vpk_sockaddr cand_addr[CAND_CNT];
    int cand_weight[CAND_CNT];
    int selected_cand;
    char strip[VPK_INET6_ADDRSTRLEN + 20];
    /* Special IPv4 addresses. */
    struct spec_ipv4_t
    {
        unsigned int addr;
        unsigned int mask;
        int weight;
    } spec_ipv4[] =
        {
            /* 127.0.0.0/8, loopback addr will be used if there is no other
             * addresses.
             */
            {0x7f000000, 0xFF000000, WEIGHT_LOOPBACK},

            /* 0.0.0.0/8, special IP that doesn't seem to be practically useful */
            {0x00000000, 0xFF000000, WEIGHT_DISABLED},

            /* 169.254.0.0/16, a zeroconf/link-local address, which has higher
             * priority than loopback and will be used if there is no other
             * valid addresses.
             */
            {0xa9fe0000, 0xFFFF0000, WEIGHT_LINK_LOCAL}};
    /* Special IPv6 addresses */
    struct spec_ipv6_t
    {
        unsigned char addr[16];
        unsigned char mask[16];
        int weight;
    } spec_ipv6[] =
        {
            /* Loopback address, ::1/128 */
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
             {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
             WEIGHT_LOOPBACK},

            /* Link local, fe80::/10 */
            {{0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
             {0xff, 0xc0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
             WEIGHT_LINK_LOCAL},

            /* Disabled, ::/128 */
            {{0x0, 0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
             {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
             WEIGHT_DISABLED}};

    vpk_addrinfo ai;
    int status;

    cand_cnt = 0;
    memset(cand_addr, 0x00, sizeof(cand_addr));
    memset(cand_weight, 0x00, sizeof(cand_weight));
    for (i = 0; i < _countof(cand_addr); ++i)
    {
        cand_addr[i].ss.sa_family = (unsigned short)af;
    }

    addr->ss.sa_family = (unsigned short)af;

#if !defined(VPK_GETHOSTIP_DISABLE_LOCAL_RESOLUTION) || VPK_GETHOSTIP_DISABLE_LOCAL_RESOLUTION == 0
    /* Get hostname's IP address */
    {
        char hostname[VPK_MAX_HOSTNAME];
        count = 1;

        char *nodename = vpk_gethostname(hostname);
        if (nodename)
            status = vpk_getaddrinfo(af, nodename, &count, &ai);
        else
            status = -18;

        if (status == 0)
        {
            //assert(ai.ai_addr.ss.sa_family == (unsigned short)af);
            return_val_if_fail(ai.ai_addr.ss.sa_family == (unsigned short)af, -1);
            vpk_addr_copy(&cand_addr[cand_cnt], &ai.ai_addr);
            vpk_sockaddr_set_port(&cand_addr[cand_cnt], 0);
            cand_weight[cand_cnt] += WEIGHT_HOSTNAME;
            ++cand_cnt;

            vpk_addr_to_string(&ai.ai_addr, strip);
            printf("hostname IP is %s\n", strip);
            //VPK_TRACE("  hostname IP is %s", get_addr(&ai.ai_addr));
        }
    }
#endif

    /* Get default interface (interface for default route) */
    if (cand_cnt < _countof(cand_addr))
    {
        status = vpk_default_ipinterface(af, addr);
        if (status == 0)
        {
            vpk_addr_to_string(addr, strip);
            printf("default IP is %s\n", strip);
            //VPK_TRACE("default IP is %s", get_addr(addr));

            vpk_sockaddr_set_port(addr, 0);
            for (i = 0; i < cand_cnt; ++i)
            {
                if (vpk_sockaddr_cmp(&cand_addr[i], addr) == 0)
                    break;
            }

            cand_weight[i] += WEIGHT_DEF_ROUTE;
            if (i >= cand_cnt)
            {
                vpk_addr_copy(&cand_addr[i], addr);
                ++cand_cnt;
            }
        }
    }

    /* Enumerate IP interfaces */
    if (cand_cnt < _countof(cand_addr))
    {
        unsigned start_if = cand_cnt;
        count = _countof(cand_addr) - start_if;

        status = vpk_enum_ip_interface(af, &count, &cand_addr[start_if]);
        if (status == 0 && count)
        {
            /* Clear the port number */
            for (i = 0; i < count; ++i)
                vpk_sockaddr_set_port(&cand_addr[start_if + i], 0);

            /**
             * For each candidate that we found so far (that is the hostname
             * address and default interface address, check if they're found
             * in the interface list. If found, add the weight, and if not,
             * decrease the weight.
             */
            for (i = 0; i < cand_cnt; ++i)
            {
                unsigned j;
                for (j = 0; j < count; ++j)
                {
                    if (vpk_sockaddr_cmp(&cand_addr[i], &cand_addr[start_if + j]) == 0)
                        break;
                }

                if (j == count)
                {
                    /* Not found */
                    cand_weight[i] -= WEIGHT_INTERFACE;
                }
                else
                {
                    cand_weight[i] += WEIGHT_INTERFACE;
                }
            }

            /* Add remaining interface to candidate list. */
            for (i = 0; i < count; ++i)
            {
                unsigned j;
                for (j = 0; j < cand_cnt; ++j)
                {
                    if (vpk_sockaddr_cmp(&cand_addr[start_if + i], &cand_addr[j]) == 0)
                        break;
                }

                if (j == cand_cnt)
                {
                    vpk_addr_copy(&cand_addr[cand_cnt], &cand_addr[start_if + i]);
                    cand_weight[cand_cnt] += WEIGHT_INTERFACE;
                    ++cand_cnt;
                }
            }
        }
    }

    /**
     * Apply weight adjustment for special IPv4/IPv6 addresses
     */
    if (af == AF_INET)
    {
        for (i = 0; i < cand_cnt; ++i)
        {
            unsigned j;
            for (j = 0; j < _countof(spec_ipv4); ++j)
            {
                unsigned int a = ntohl(cand_addr[i].s4.sin_addr.s_addr);
                unsigned int pa = spec_ipv4[j].addr;
                unsigned int pm = spec_ipv4[j].mask;

                if ((a & pm) == pa)
                {
                    cand_weight[i] += spec_ipv4[j].weight;
                    break;
                }
            }
        }
    }
    else if (af == AF_INET6)
    {
        for (i = 0; i < _countof(spec_ipv6); ++i)
        {
            unsigned j;
            for (j = 0; j < cand_cnt; ++j)
            {
                unsigned char *a = cand_addr[j].s6.sin6_addr.s6_addr;
                unsigned char am[16];
                unsigned char *pa = spec_ipv6[i].addr;
                unsigned char *pm = spec_ipv6[i].mask;
                unsigned k;

                for (k = 0; k < 16; ++k)
                {
                    am[k] = (unsigned char)((a[k] & pm[k]) & 0xFF);
                }

                if (memcmp(am, pa, 16) == 0)
                {
                    cand_weight[j] += spec_ipv6[i].weight;
                }
            }
        }
    }
    else
    {
        return -1;
    }

    /* Enumerate candidates to get the best IP address to choose */
    selected_cand = -1;
    for (i = 0; i < cand_cnt; ++i)
    {
        vpk_addr_to_string(&cand_addr[i], strip);
        printf("Checking candidate IP %s, weight=%d\n", strip, cand_weight[i]);

        if (cand_weight[i] < MIN_WEIGHT)
        {
            continue;
        }

        if (selected_cand == -1)
            selected_cand = i;
        else if (cand_weight[i] > cand_weight[selected_cand])
            selected_cand = i;
    }

    /* If else fails, returns loopback interface as the last resort */
    if (selected_cand == -1)
    {
        if (af == AF_INET)
        {
            addr->s4.sin_addr.s_addr = htonl(0x7f000001);
        }
        else
        {
            struct in6_addr *sin6_addr = vpk_sockaddr_get_addr(addr);
            memset(sin6_addr, 0x00, sizeof(struct in6_addr));
            sin6_addr->s6_addr[15] = 1;
        }

        vpk_addr_to_string(addr, strip);
        printf("Loopback IP %s returned\n", strip);
    }
    else
    {
        vpk_addr_copy(addr, &cand_addr[selected_cand]);
        vpk_addr_to_string(addr, strip);
        printf("Candidate %s selected\n", strip);
    }

    return 0;
}
