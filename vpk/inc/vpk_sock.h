/**
 * History:
 * ================================================================
 * 2019-05-28 qing.zou created
 *
 */

#ifndef VPK_SOCK_H
#define VPK_SOCK_H

#include <netinet/in.h>

#include "vpk_typedef.h"

VPK_BEGIN_DELS

#define VPK_SOCK_HAS_GETADDRINFO 0

/** Address to accept any incoming messages. */
#define VPK_INADDR_ANY              ((unsigned int)0)

/** Address indicating an error return */
#define VPK_INADDR_NONE             ((unsigned int)0xffffffff)

/** Address to send to all hosts. */
#define VPK_INADDR_BROADCAST        ((unsigned int)0xffffffff)

/* TTL */
#define TTL_IGNORE                  ((int)(-1))
#define TTL_DEFAULT                 (64)

/* TOS */
#define TOS_IGNORE                  ((int)(-1))
#define TOS_DEFAULT                 (0)

#define VPK_SOCKET_MAGIC            (0xABACADEF)

#define VPK_MAX_ADDR_STRING             (65)
#define DEFAULT_CLIENT_SOCK_BUF_SIZE    (65536)
#define VPK_CMSG_MAX_SIZE               (65536)

typedef union {
    struct sockaddr         ss;
    struct sockaddr_in      s4;
    struct sockaddr_in6     s6;
} vpk_sockaddr;

/**
 * Maximum length of text representation of an IPv4 address.
 */
#define VPK_INET_ADDRSTRLEN             16

/**
 * Maximum length of text representation of an IPv6 address.
 */
#define VPK_INET6_ADDRSTRLEN            46

/**
 * Maximum hostname length.
 */
#define VPK_MAX_HOSTNAME                (128)

/** This structure describes an Internet host address. */
typedef struct vpk_hostent
{
    char       *h_name;         /**< The official name of the host. */
    char      **h_aliases;      /**< Aliases list. */
    int         h_addrtype;     /**< Host address type. */
    int         h_length;       /**< Length of address. */
    char      **h_addr_list;    /**< List of addresses. */
} vpk_hostent;

/**
 * This structure describes address information vpk_getaddrinfo().
 */
typedef struct vpk_addrinfo
{
    char            ai_canonname[VPK_MAX_HOSTNAME];     /** Canonical name for host */
    vpk_sockaddr    ai_addr;                            /** Binary address. */
} vpk_addrinfo;

typedef enum vpk_prototype_e
{
    VPK_PROTOTYPE_UNKNOWN   = 0,
    VPK_PROTOTYPE_TCP       = 6,
    VPK_PROTOTYPE_UDP       = 17,
    VPK_PROTOTYPE_TLS       = 56,
    VPK_PROTOTYPE_SCTP      = 132,
} vpk_prototype_t;

typedef enum sock_application_e
{
    VPK_APPTYPE_SOCKET_UNKNOWN,
    VPK_APPTYPE_SOCKET_LISTENER,
    VPK_APPTYPE_SOCKET_CLIENT,
    VPK_APPTYPE_SOCKET_RELAY,
    VPK_APPTYPE_WEBSOCKET_CLIENT,
    VPK_APPTYPE_WEBSOCKET_RELAY,
} sock_application_t;

typedef struct vpk_socket_s
{
    long                        fd;
    struct vpk_socket_s        *parent_sock;
    int                         family;
    vpk_prototype_t             protocol;
    uint32_t                    magic;
    sock_application_t          apptype;

    int                         current_ttl;
    int                         current_tos;

    void                       *e;
} vpk_socket_t;

#define vpk_socketio_t  vpk_socket_t


int vpk_sockaddr_in_set_str_addr(struct sockaddr_in *addr, const char *str_addr);
int vpk_sockaddr_set_str_addr(int af, vpk_sockaddr *addr, const char *str_addr);
int vpk_sockaddr_in_init(struct sockaddr_in *addr, const char *str_addr, unsigned short port);
VPKAPI int vpk_sockaddr_init(int af, vpk_sockaddr *addr, const char *str, unsigned short port);
VPKAPI int vpk_sockaddr_cmp(const vpk_sockaddr *addr1, const vpk_sockaddr *addr2);
VPKAPI void *vpk_sockaddr_get_addr(const vpk_sockaddr *addr);
VPKAPI unsigned short vpk_sockaddr_get_port(const vpk_sockaddr *addr);
int vpk_sockaddr_set_port(vpk_sockaddr *addr, unsigned short port);
VPKAPI int vpk_inet_pton(int af, const char *src, void *dst);
VPKAPI int vpk_inet_ntop(int af, const void *src, char *dst, int size);

unsigned int get_sockaddr_len(const vpk_sockaddr *addr);

void vpk_addr_copy(vpk_sockaddr *dst, const vpk_sockaddr *src);
int vpk_addr_bind(int fd, const vpk_sockaddr *addr, int reusable);
int vpk_addr_to_string(const vpk_sockaddr *addr, char *saddr);
char *vpk_addr_to_string2(const vpk_sockaddr *addr, char *saddr);
int vpk_addr_get_from_sock(int fd, vpk_sockaddr *addr);

/** vpk_socket **/
int vpk_socket_sock(void *e, int family, vpk_prototype_t type, vpk_socket_t *sock);
int vpk_socket_connect(long fd, const vpk_sockaddr *addr, int len);
int vpk_socket_close(long fd);

VPKAPI void *vpk_socket_ioa_create(void *e, int family, vpk_prototype_t type, vpk_socket_t *sock);
VPKAPI int vpk_socket_set_ttltos(vpk_socket_t *s);
VPKAPI int vpk_socket_closeonexec(int fd);
VPKAPI int vpk_socket_nonblocking(int fd);
VPKAPI int vpk_socket_set_reusable(int fd, int flag);
VPKAPI int vpk_socket_set_bufsize(int fd, int sz0);
VPKAPI int vpk_socket_set_ttl(int fd, int ttl);

int vpk_udp_send(int fd, const vpk_sockaddr *dest_addr, const char *buffer, int len);
int vpk_udp_recvfrom(int fd, vpk_sockaddr *orig_addr, const vpk_sockaddr *like_addr, char *buffer, int buf_size, int *ttl, int *tos, char *ecmsg, int flags, uint32_t *errcode);

/** addrinfo resolv **/
int vpk_getaddrinfo(int af, const char *nodename, unsigned *count, vpk_addrinfo ai[]);
char *vpk_gethostname(char *hostname);

/** ip interface **/
int vpk_getipinterface(int af, const char *dst, vpk_sockaddr *if_addr, int allow_resolve, vpk_sockaddr *p_dst_addr);
int vpk_default_ipinterface(int af, vpk_sockaddr *addr);

#define VPK_SOCK_IGNORE_LOOPBACK_IF     0
int vpk_enum_if_by_af(int af, unsigned *count, vpk_sockaddr ifs[]);
int vpk_enum_ip_interface(int af, unsigned *count, vpk_sockaddr ifs[]);

VPK_END_DELS

#endif //VPK_SOCK_H
