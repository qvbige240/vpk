/**
 * History:
 * ================================================================
 * 2019-05-28 qing.zou created
 *
 */
 
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>

#include <arpa/inet.h>

#include "vpk_sock.h"
#include "vpk_logging.h"


int vpk_socket_closeonexec(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFD, NULL)) < 0) {
		printf("fcntl(%d, F_GETFD)", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
		printf("fcntl(%d, F_SETFD)", fd);
		return -1;
	}

	return 0;
}

int vpk_socket_nonblocking(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
		printf("fcntl(%d, F_GETFL)", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		printf("fcntl(%d, F_GETFL)", fd);
		return -1;
	}

	return 0;
}

void* vpk_sockaddr_get_addr(const vpk_sockaddr *addr)
{
	const vpk_sockaddr *a = (const vpk_sockaddr*)addr;

	return_val_if_fail(a->ss.sa_family == AF_INET ||
		a->ss.sa_family == AF_INET6, NULL);

	if (a->ss.sa_family == AF_INET6)
		return (void*) &a->s6.sin6_addr;
	else
		return (void*) &a->s4.sin_addr;
}

unsigned short vpk_sockaddr_get_port(const vpk_sockaddr *addr)
{
	const vpk_sockaddr *a = (const vpk_sockaddr*) addr;

	return_val_if_fail(a->ss.sa_family == AF_INET ||
		a->ss.sa_family == AF_INET6, 0);

	return ntohs((unsigned short)(a->ss.sa_family == AF_INET6 ?
		a->s6.sin6_port : a->s4.sin_port));
}

int vpk_sockaddr_set_port(vpk_sockaddr *addr, unsigned short port)
{
	vpk_sockaddr *a = addr;

	return_val_if_fail(a || port > 0, -1);
	if (a->ss.sa_family == AF_INET)
		a->s4.sin_port = htons(port);
	else if (a->ss.sa_family == AF_INET6)
		a->s6.sin6_port = htons(port);

	return 0;
}

int vpk_inet_ntop(int af, const void *src, char *dst, int size)
{
	return_val_if_fail(src && dst && size, -1);

	*dst = '\0';

	return_val_if_fail(af==AF_INET || af==AF_INET6, -2);

	if (inet_ntop(af, src, dst, size) == NULL) {
		int status = errno;
		if (status == 0)
			status = -3;

		return status;
	}

	return 0;
}

void vpk_addr_copy(vpk_sockaddr* dst, const vpk_sockaddr* src)
{
	if(dst && src)
		vpk_bcopy(src, dst, sizeof(vpk_sockaddr));
}

unsigned int get_sockaddr_len(const vpk_sockaddr* addr)
{
	if (addr->ss.sa_family == AF_INET) return sizeof(struct sockaddr_in);
	else if (addr->ss.sa_family == AF_INET6) return sizeof(struct sockaddr_in6);
	return 0;
}

int vpk_addr_get_from_sock(int fd, vpk_sockaddr *addr)
{
	return_val_if_fail(fd >= 0 && addr, -1);

	vpk_sockaddr a;
	a.ss.sa_family = AF_INET6;
	socklen_t socklen = get_sockaddr_len(&a);
	if (getsockname(fd, (struct sockaddr*) &a, &socklen) < 0) {
		a.ss.sa_family = AF_INET;
		socklen = get_sockaddr_len(&a);
		if (getsockname(fd, (struct sockaddr*) &a, &socklen) < 0) {
			return -1;
		}
	}

	//vpk_bcopy(&a, addr, sizeof(vpk_sockaddr));
	vpk_addr_copy(addr, &a);

	return 0;
}


#define CORRECT_RAW_TTL(ttl) do { if(ttl<0 || ttl>255) ttl=TTL_DEFAULT; } while(0)
#define CORRECT_RAW_TOS(tos) do { if(tos<0 || tos>255) tos=TOS_DEFAULT; } while(0)

typedef unsigned char recv_ttl_t;
typedef unsigned char recv_tos_t;


int vpk_sock_buf_size_set(int fd, int sz0)
{
	int ret = 0, sz = sz0;
	while (sz > 0) {
		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const void*)(&sz), (socklen_t)sizeof(sz)) < 0) {
			sz = sz / 2;
		} else {
			break;
		}
	}

	if (sz < 1) {
		printf("Cannot set recv sock size %d on fd %d\n", sz0, fd);
		ret = -1;
	}

	sz = sz0;
	while (sz > 0) {
		if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const void*)(&sz), (socklen_t)sizeof(sz)) < 0) {
			sz = sz / 2;
		} else {
			break;
		}
	}

	if (sz < 1) {
		printf("Cannot set send sock size %d on fd %d\n", sz0, fd);
		ret = -1;
	}

	return ret;
}

static int set_socket_ttl_options(int fd, int family)
{
	if (family == AF_INET) {
#if defined(IP_RECVTTL)
		int recv_ttl_on = 1;
		if (setsockopt(fd, IPPROTO_IP, IP_RECVTTL, &recv_ttl_on, sizeof(recv_ttl_on)) < 0) {
			perror("cannot set IP_RECVTTL\n");
		}
#endif
	}

	return 0;
}

static int set_socket_tos_options(int fd, int family)
{
	if (family == AF_INET) {
#if defined(IP_RECVTOS)
		int recv_tos_on = 1;
		if (setsockopt(fd, IPPROTO_IP, IP_RECVTOS, &recv_tos_on, sizeof(recv_tos_on)) < 0) {
			perror("cannot set IP_RECVTOS\n");
		}
#endif
	}

	return 0;
}

static int get_raw_socket_ttl(int fd, int family)
{
	int ttl = 0;

	if(family == AF_INET) {
#if !defined(IP_TTL)
		UNUSED_ARG(fd);
		do { return TTL_IGNORE; } while(0);
#else
		socklen_t slen = (socklen_t)sizeof(ttl);
		if(getsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, &slen) < 0) {
			perror("get TTL on socket");
			return TTL_IGNORE;
		}
#endif
	}

	CORRECT_RAW_TTL(ttl);

	return ttl;
}

static int get_raw_socket_tos(int fd, int family)
{
	int tos = 0;

	if(family == AF_INET) {
#if !defined(IP_TOS)
		UNUSED_ARG(fd);
		do { return TOS_IGNORE; } while(0);
#else
		socklen_t slen = (socklen_t)sizeof(tos);
		if(getsockopt(fd, IPPROTO_IP, IP_TOS, &tos, &slen) < 0) {
			perror("get TOS on socket");
			return -1;
		}
#endif
	}

	CORRECT_RAW_TOS(tos);

	return tos;
}

int vpk_socket_set_ttl(int fd, int ttl)
{
	if(setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		perror("set TTL on socket");
		return -1;
	}
	printf("set ttl %d\n", ttl);

	return 0;
}

int vpk_socket_set_reusable(int fd, int flag)
{
	if (fd < 0)
		return -1;
	else {
		int use_reuseaddr = 1;

		if (use_reuseaddr) {
			int on = flag;
			int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*) &on, (socklen_t)sizeof(on));
			if (ret < 0)
				perror("SO_REUSEADDR");
		}

		//if (use_reuseaddr) {
		//	int on = flag;
		//	setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const void*) &on, (socklen_t) sizeof(on));
		//}

		return 0;
	}
}

static int set_socket_options_fd(int fd, vpk_prototype_t type, int family)
{
	if (fd < 0)
		return 0;

	vpk_sock_buf_size_set(fd, DEFAULT_CLIENT_SOCK_BUF_SIZE);

	vpk_socket_nonblocking(fd);

	if (type == VPK_PROTOTYPE_UDP)
	{
		set_socket_ttl_options(fd, family);
		set_socket_tos_options(fd, family);
	}

	return 0;
}

static int set_socket_options(vpk_socketio_t *s)
{
	if(!s || (s->parent_sock))
		return 0;

	set_socket_options_fd(s->fd, s->protocol, s->family);

	s->current_ttl = get_raw_socket_ttl(s->fd, s->family);
	s->current_tos = get_raw_socket_tos(s->fd, s->family);

	return 0;
}

void* vpk_socket_ioa_create(void *e, int family, vpk_prototype_t type, vpk_socketio_t *sock)
{
	int fd = -1;
	vpk_socketio_t *s = sock;

	switch (type) {
		case VPK_PROTOTYPE_UDP:
			fd = socket(/*PF_INET*/ family, SOCK_DGRAM, IPPROTO_IP);
			if (fd < 0) {
				perror("UDP socket");
				return NULL;
			}
			vpk_sock_buf_size_set(fd, DEFAULT_CLIENT_SOCK_BUF_SIZE);
			break;
		case VPK_PROTOTYPE_TCP:
			fd = socket(family, SOCK_STREAM, IPPROTO_IP);
			if (fd < 0) {
				perror("TCP socket");
				return NULL;
			}
			vpk_sock_buf_size_set(fd, DEFAULT_CLIENT_SOCK_BUF_SIZE);
			break;
		default:break;
	}

	s->magic	= VPK_SOCKET_MAGIC;
	s->fd		= fd;
	s->family	= family;
	s->e		= e;

	set_socket_options(s);

	return s;
}

int vpk_addr_to_string(const vpk_sockaddr* addr, unsigned char* saddr)
{
	if (addr && saddr) {

		char addrtmp[VPK_MAX_ADDR_STRING];

		if (addr->ss.sa_family == AF_INET) {
			inet_ntop(AF_INET, &addr->s4.sin_addr, addrtmp, INET_ADDRSTRLEN);
			if(vpk_sockaddr_get_port(addr) > 0)
				snprintf((char*)saddr, VPK_MAX_ADDR_STRING, "%s:%d", addrtmp, vpk_sockaddr_get_port(addr));
			else
				strncpy((char*)saddr, addrtmp, VPK_MAX_ADDR_STRING);
		} else if (addr->ss.sa_family == AF_INET6) {
			inet_ntop(AF_INET6, &addr->s6.sin6_addr, addrtmp, INET6_ADDRSTRLEN);
			if(vpk_sockaddr_get_port(addr) > 0)
				snprintf((char*)saddr, VPK_MAX_ADDR_STRING, "[%s]:%d", addrtmp, vpk_sockaddr_get_port(addr));
			else
				strncpy((char*)saddr, addrtmp, VPK_MAX_ADDR_STRING);
		} else {
			return -1;
		}

		return 0;
	}

	return -1;
}

int vpk_addr_bind(int fd, const vpk_sockaddr* addr, int reusable)
{
	if (!addr || fd < 0) {

		return -1;

	} else {

		int ret = -1;

		vpk_socket_set_reusable(fd, reusable);

		if (addr->ss.sa_family == AF_INET) {
			do {
				ret = bind(fd, (const struct sockaddr *) addr, sizeof(struct sockaddr_in));
			} while (ret < 0 && errno == EINTR);
		} else if (addr->ss.sa_family == AF_INET6) {
			const int off = 0;
			setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char *) &off, sizeof(off));
			do {
				ret = bind(fd, (const struct sockaddr *) addr, sizeof(struct sockaddr_in6));
			} while (ret < 0 && errno == EINTR);
		} else {
			return -1;
		}

		if(ret < 0) {

			//char ip[INET_ADDRSTRLEN] = {0};
			//vpk_inet_ntop(AF_INET, vpk_sockaddr_get_addr(addr), ip, sizeof(ip));
			//printf("ip: %s:%u\n", ip, vpk_sockaddr_get_port(addr));

			int err = errno;
			perror("bind");
			char str[129];
			vpk_addr_to_string(addr,(unsigned char*)str);
			printf("Trying to bind fd %d to <%s>: errno=%d\n", fd, str, err);

		}
		return ret;
	}
}


static int is_connreset(void) {
	switch (errno) {
	case ECONNRESET:
	case ECONNREFUSED:
		return 1;
	default:
		;
	}
	return 0;
}


#define CMSG_MAX_SIZE		VPK_CMSG_MAX_SIZE
#define MAX_ERRORS_IN_UDP_BATCH (1024)

#define SOCK_TRIAL_EFFORTS_TO_SEND (2)

static int socket_readerr(int fd, vpk_sockaddr *orig_addr)
{
	if ((fd < 0) || !orig_addr)
		return -1;

#if defined(CMSG_SPACE) && defined(MSG_ERRQUEUE) && defined(IP_RECVERR)

	unsigned char ecmsg[CMSG_MAX_SIZE];
	int flags = MSG_ERRQUEUE;
	int len = 0;

	struct msghdr msg;
	struct iovec iov;
	char buffer[CMSG_MAX_SIZE];		//... 4*1024

	char *cmsg = (char*)ecmsg;

	msg.msg_control = cmsg;
	msg.msg_controllen = CMSG_MAX_SIZE;
	/* CMSG_SPACE(sizeof(recv_ttl)+sizeof(recv_tos)) */

	msg.msg_name = orig_addr;
	msg.msg_namelen = (socklen_t)get_sockaddr_len(orig_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = buffer;
	msg.msg_iov->iov_len = sizeof(buffer);
	msg.msg_flags = 0;

	int try_cycle = 0;

	do {
		do {
			len = recvmsg(fd, &msg, flags);
		} while (len < 0 && (errno == EINTR));

	} while((len > 0)&&(try_cycle++ < MAX_ERRORS_IN_UDP_BATCH));

#endif

	return 0;
}

int vpk_udp_send(int fd, const vpk_sockaddr* dest_addr, const char* buffer, int len)
{
	int rc = 0;

	if (fd >= 0) {

		int cycle;
		int try_again = 1;
		//#if !defined(TURN_IP_RECVERR)
		//		try_again = 0;
		//#endif

try_start:
		cycle = 0;

		if (dest_addr) {

			int slen = get_sockaddr_len(dest_addr);

			do {
				rc = sendto(fd, buffer, len, 0, (const struct sockaddr*) dest_addr, (socklen_t) slen);
			} while (
				((rc < 0) && (errno == EINTR)) ||
				((rc<0) && is_connreset() && (++cycle < SOCK_TRIAL_EFFORTS_TO_SEND))
				);

		} else {
			//do {
			//	rc = send(fd, buffer, len, 0);
			//} while (
			//	((rc < 0) && (errno == EINTR)) ||
			//	((rc<0) && is_connreset() && (++cycle<SOCK_TRIAL_EFFORTS_TO_SEND))
			//	);
		}

		if(rc < 0) {
			if((errno == ENOBUFS) || (errno == EAGAIN)) {
				//Lost packet due to overload ... fine.
				rc = len;
			} else if(is_connreset()) {
				if(try_again) {
					try_again = 0;
					LOG_D("UDP Socket, tring to recover write operation...\n");
					//socket_readerr(fd, &(s->local_addr));
					goto try_start;
				}
				//Lost packet - sent to nowhere... fine.
				rc = len;
			}
		}
	}

	return rc;
}

int vpk_udp_recvfrom(int fd, vpk_sockaddr* orig_addr, const vpk_sockaddr *like_addr, char* buffer, int buf_size, int *ttl, int *tos, char *ecmsg, int flags, uint32_t *errcode)
{
	int len = 0;

	if (fd < 0 || !orig_addr || !like_addr || !buffer)
		return -1;

	if(errcode)
		*errcode = 0;

	int slen = get_sockaddr_len(like_addr);
	recv_ttl_t recv_ttl = TTL_DEFAULT;
	recv_tos_t recv_tos = TOS_DEFAULT;

#if 0 //!defined(CMSG_SPACE)
	do {
		len = recvfrom(fd, buffer, buf_size, flags, (struct sockaddr*) orig_addr, (socklen_t*) &slen);
	} while (len < 0 && (errno == EINTR));

	if (len < 0 && errcode)
		*errcode = (uint32_t)errno;
#else
	struct msghdr msg;
	struct iovec iov;

	char *cmsg = (char*)ecmsg;

	msg.msg_control = cmsg;
	msg.msg_controllen = CMSG_MAX_SIZE;
	/* CMSG_SPACE(sizeof(recv_ttl)+sizeof(recv_tos)) */

	msg.msg_name = orig_addr;
	msg.msg_namelen = (socklen_t)slen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = buffer;
	msg.msg_iov->iov_len = (size_t)buf_size;
	msg.msg_flags = 0;

#if defined(MSG_ERRQUEUE)
	int try_cycle = 0;
try_again:
#endif

	do {
		len = recvmsg(fd, &msg, flags);
	} while (len < 0 && (errno == EINTR));

#if defined(MSG_ERRQUEUE)

	if(flags & MSG_ERRQUEUE) {
		if((len > 0) && (try_cycle++ < MAX_ERRORS_IN_UDP_BATCH)) goto try_again;
	}

	if((len < 0) && (!(flags & MSG_ERRQUEUE))) {
		//Linux
		int eflags = MSG_ERRQUEUE | MSG_DONTWAIT;
		uint32_t errcode1 = 0;
		vpk_udp_recvfrom(fd, orig_addr, like_addr, buffer, buf_size, ttl, tos, ecmsg, eflags, &errcode1);
		//try again...
		do {
			len = recvmsg(fd, &msg, flags);
		} while (len < 0 && (errno == EINTR));
	}
#endif

	if (len >= 0) {

		struct cmsghdr *cmsgh;

		// Receive auxiliary data in msg
		for (cmsgh = CMSG_FIRSTHDR(&msg); cmsgh != NULL; cmsgh = CMSG_NXTHDR(&msg,cmsgh)) {
			int l = cmsgh->cmsg_level;
			int t = cmsgh->cmsg_type;

			switch(l) 
			{
			case IPPROTO_IP:
				switch(t) 
				{
#if defined(IP_RECVTTL)
				case IP_RECVTTL:
				case IP_TTL:
					recv_ttl = *((recv_ttl_t *) CMSG_DATA(cmsgh));
					break;
#endif
#if defined(IP_RECVTOS)
				case IP_RECVTOS:
				case IP_TOS:
					recv_tos = *((recv_tos_t *) CMSG_DATA(cmsgh));
					break;
#endif

					//#if defined(IP_RECVERR)
					//					case IP_RECVERR:
					//						{
					//							struct turn_sock_extended_err *e=(struct turn_sock_extended_err*) CMSG_DATA(cmsgh);
					//							if(errcode)
					//								*errcode = e->ee_errno;
					//						}
					//						break;
					//#endif
				default:
					;
					/* no break */
				};
				break;
			case IPPROTO_IPV6:
				//					switch(t) {
				//#if defined(IPV6_RECVHOPLIMIT)
				//				case IPV6_RECVHOPLIMIT:
				//				case IPV6_HOPLIMIT:
				//					recv_ttl = *((recv_ttl_t *) CMSG_DATA(cmsgh));
				//					break;
				//#endif
				//#if defined(IPV6_RECVTCLASS)
				//				case IPV6_RECVTCLASS:
				//				case IPV6_TCLASS:
				//					recv_tos = *((recv_tos_t *) CMSG_DATA(cmsgh));
				//					break;
				//#endif
				//#if defined(IPV6_RECVERR)
				//				case IPV6_RECVERR:
				//					{
				//						struct turn_sock_extended_err *e=(struct turn_sock_extended_err*) CMSG_DATA(cmsgh);
				//						if(errcode)
				//							*errcode = e->ee_errno;
				//					}
				//					break;
				//#endif
				//				default:
				//					;
				//					/* no break */
				//					};
				break;
			default:
				;
				/* no break */
			};
		}
	}

#endif

	*ttl = recv_ttl;

	CORRECT_RAW_TTL(*ttl);

	*tos = recv_tos;

	CORRECT_RAW_TOS(*tos);

	return len;
}
