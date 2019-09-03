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

TIMA_BEGIN_DELS

/* TTL */
#define TTL_IGNORE ((int)(-1))
#define TTL_DEFAULT (64)

/* TOS */
#define TOS_IGNORE ((int)(-1))
#define TOS_DEFAULT (0)


#define VPK_SOCKET_MAGIC				(0xABACADEF)

#define VPK_MAX_ADDR_STRING				(65)
#define DEFAULT_CLIENT_SOCK_BUF_SIZE	(65536)
#define VPK_CMSG_MAX_SIZE				(65536)

typedef union {
	struct sockaddr ss;
	struct sockaddr_in s4;
	struct sockaddr_in6 s6;
} vpk_sockaddr;

typedef enum vpk_prototype_e
{
	VPK_PROTOTYPE_UNKNOWN = 0,
	VPK_PROTOTYPE_TCP		= 6,
	VPK_PROTOTYPE_UDP		= 17,
	VPK_PROTOTYPE_TLS		= 56,
	VPK_PROTOTYPE_SCTP		= 132,
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

typedef struct vpk_socketio_s
{
	int						fd;
	struct vpk_socketio_s	*parent_sock;
	int						family;
	vpk_prototype_t			protocol;
	uint32_t				magic;
	sock_application_t		apptype;

	int						current_ttl;
	int						current_tos;

	void					*e;
} vpk_socketio_t;


VPKAPI int vpk_socket_closeonexec(int fd);
VPKAPI int vpk_socket_nonblocking(int fd);
VPKAPI void* vpk_sockaddr_get_addr(const vpk_sockaddr *addr);
VPKAPI unsigned short vpk_sockaddr_get_port(const vpk_sockaddr *addr);
int vpk_sockaddr_set_port(vpk_sockaddr *addr, unsigned short port);
VPKAPI int vpk_inet_ntop(int af, const void *src, char *dst, int size);

unsigned int get_sockaddr_len(const vpk_sockaddr* addr);

void vpk_addr_copy(vpk_sockaddr* dst, const vpk_sockaddr* src);
int vpk_addr_bind(int fd, const vpk_sockaddr* addr, int reusable);
int vpk_addr_to_string(const vpk_sockaddr* addr, unsigned char* saddr);
int vpk_addr_get_from_sock(int fd, vpk_sockaddr *addr);

int vpk_sock_buf_size_set(int fd, int sz0);

int vpk_socket_set_ttl(int fd, int ttl);
int vpk_socket_set_reusable(int fd, int flag);

void* vpk_socket_ioa_create(void *e, int family, vpk_prototype_t type, vpk_socketio_t *sock);

int vpk_udp_send(int fd, const vpk_sockaddr* dest_addr, const char* buffer, int len);
int vpk_udp_recvfrom(int fd, vpk_sockaddr* orig_addr, const vpk_sockaddr *like_addr, char* buffer, int buf_size, int *ttl, int *tos, char *ecmsg, int flags, uint32_t *errcode);

TIMA_END_DELS

#endif //VPK_SOCK_H
