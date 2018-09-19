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
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "vpk.h"

#define	MSG_ERRQUEUE	MSG_ERRQUEUE
#define MAX_ERRORS_IN_UDP_BATCH (1024)

#define TRIAL_EFFORTS_TO_SEND (2)

/* TTL */
#define TTL_IGNORE ((int)(-1))
#define TTL_DEFAULT (64)

/* TOS */
#define TOS_IGNORE ((int)(-1))
#define TOS_DEFAULT (0)

#define CORRECT_RAW_TTL(ttl) do { if(ttl<0 || ttl>255) ttl=TTL_DEFAULT; } while(0)
#define CORRECT_RAW_TOS(tos) do { if(tos<0 || tos>255) tos=TOS_DEFAULT; } while(0)

typedef unsigned char recv_ttl_t;
typedef unsigned char recv_tos_t;

typedef union {
	struct sockaddr ss;
	struct sockaddr_in s4;
	struct sockaddr_in6 s6;
} vpk_sockaddr;

char cmsg[65536];

uint32_t get_sockaddr_len(const vpk_sockaddr* addr) {
	if(addr->ss.sa_family == AF_INET) return sizeof(struct sockaddr_in);
	else if(addr->ss.sa_family == AF_INET6) return sizeof(struct sockaddr_in6);
	return 0;
}

void addr_cpy(vpk_sockaddr* dst, const vpk_sockaddr* src) {
	if(dst && src)
		bcopy(src, dst, sizeof(vpk_sockaddr));
}

int addr_get_from_sock(int fd, vpk_sockaddr *addr)
{
	if (fd < 0 || !addr)
		return -1;
	else {

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

		LOG_D("getsockname: %s:%d\n", inet_ntoa(a.s4.sin_addr), ntohs(a.s4.sin_port));
		addr_cpy(addr, &a);

		return 0;
	}
}

int is_connreset(void) {
	switch (errno) {
	case ECONNRESET:
	case ECONNREFUSED:
		return 1;
	default:
		;
	}
	return 0;
}

int socket_readerr(int fd, vpk_sockaddr *orig_addr)
{
	if ((fd < 0) || !orig_addr)
		return -1;

#if 1//defined(CMSG_SPACE) && defined(MSG_ERRQUEUE) && defined(IP_RECVERR)

	unsigned char ecmsg[65536];
	int flags = MSG_ERRQUEUE;
	int len = 0;

	struct msghdr msg;
	struct iovec iov;
	char buffer[65536];

	char *cmsg = (char*)ecmsg;

	msg.msg_control = cmsg;
	msg.msg_controllen = 65536;
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

int udp_send(int fd, const vpk_sockaddr* dest_addr, const char* buffer, int len)
{
	int rc = 0;

	if(fd >= 0) {

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
				((rc<0) && is_connreset() && (++cycle < TRIAL_EFFORTS_TO_SEND))
				);

		} else {
			//do {
			//	rc = send(fd, buffer, len, 0);
			//} while (
			//	((rc < 0) && (errno == EINTR)) ||
			//	((rc<0) && is_connreset() && (++cycle<TRIAL_EFFORTS_TO_SEND))
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


int udp_recvfrom(int fd, vpk_sockaddr* orig_addr, const vpk_sockaddr *like_addr, char* buffer, int buf_size, int *ttl, int *tos, char *ecmsg, int flags, uint32_t *errcode)
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
	msg.msg_controllen = 65536;
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
		udp_recvfrom(fd, orig_addr, like_addr, buffer, buf_size, ttl, tos, ecmsg, eflags, &errcode1);
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
						//#if defined(IP_RECVTTL)
					case IP_RECVTTL:
					case IP_TTL:
						recv_ttl = *((recv_ttl_t *) CMSG_DATA(cmsgh));
						break;
						//#endif
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

static int socket_nonblocking(int fd)
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

int set_sock_buf_size(int fd, int sz0)
{
	int sz;

	sz = sz0;
	while (sz > 0) {
		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const void*) (&sz), (socklen_t) sizeof(sz)) < 0) {
			sz = sz / 2;
		} else {
			break;
		}
	}
	if (sz < 1) {
		perror("Cannot set socket rcv size");
		printf("Cannot set rcv sock size %d on fd %d\n", sz0, fd);
	}

	sz = sz0;
	while (sz > 0) {
		if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const void*) (&sz), (socklen_t) sizeof(sz)) < 0) {
			sz = sz / 2;
		} else {
			break;
		}
	}
	if (sz < 1) {
		perror("Cannot set socket snd size");
		printf("Cannot set snd sock size %d on fd %d\n", sz0, fd);
	}

	return 0;
}

int set_socket_ttl_options(int fd, int family)
{
	if (family == AF_INET) {
#if 1//defined(IP_RECVTTL)
		int recv_ttl_on = 1;
		if (setsockopt(fd, IPPROTO_IP, IP_RECVTTL, &recv_ttl_on, sizeof(recv_ttl_on)) < 0) {
				perror("cannot set recvttl\n");
		}
#endif
	}

	return 0;
}


int set_socket_ttl(int fd, int ttl)
{
	if(setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl,sizeof(ttl)) < 0) {
		perror("set TTL on socket");
		return -1;
	}
	LOG_D("set ttl %d", ttl);

	return 0;
}

int socket_set_reusable(int fd, int flag)
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

		if (use_reuseaddr) {
			int on = flag;
			setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const void*) &on, (socklen_t) sizeof(on));
		}

		return 0;
	}
}
static int test_socket_create(int lport)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1)
	{
		printf("socket error\n");
		return -1;
	}
	set_sock_buf_size(fd, 1024);
	socket_nonblocking(fd);
	set_socket_ttl_options(fd, AF_INET);
	socket_set_reusable(fd, 1);

	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));  
	my_addr.sin_family = PF_INET;  
	my_addr.sin_port = htons(lport);  
	my_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {  
		perror("bind local");  
		exit(1);  
	} else {  
		printf("local bind OK\n");  
	} 

	return fd;
}

#define MAXBUF			1024
#define MAXEPOLLSIZE	100  

static int main_ttl_server(int lport)
{
	int n, nfds, kdpfd = 0;	
	struct epoll_event ev;  
	struct epoll_event events[MAXEPOLLSIZE];

	int ttl = TTL_IGNORE;
	int tos = TOS_IGNORE;
	char cmsg[65536];
	struct sockaddr_in remote_addr;
	struct sockaddr_in my_addr;
	char recv_buff[MAXBUF] = {0};

	int server = test_socket_create(lport);

	addr_get_from_sock(server, (vpk_sockaddr*)&my_addr);

	kdpfd = epoll_create(MAXEPOLLSIZE);  

	ev.events = EPOLLIN | EPOLLET;  
	ev.data.fd = server;  

	if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, server, &ev) < 0) {  
		fprintf(stderr, "epoll set insertion error: fd=%dn", server);  
		return -1;  
	} else {  
		printf("ep add OK\n");  
	}

	while (1) {  

		nfds = epoll_wait(kdpfd, events, 10000, -1);  
		if (nfds == -1) {  
			perror("epoll_wait");  
			break;  
		}  

		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == server) {  
				printf("server:%d\n", n);
				udp_recvfrom(server, (vpk_sockaddr*)&remote_addr, (vpk_sockaddr*)&my_addr, recv_buff, MAXBUF, &ttl, &tos, cmsg, 0, NULL);
				LOG_D("TTL: %d\n", ttl);
				//read_data(events[n].data.fd);
			}  
		}  
	}

	return 0;
}

static int main_ttl_client(const char* ip, int port, int lport)
{
	const char* msg = "hello world";
	struct sockaddr_in remote_addr;
	int client = test_socket_create(lport);
	set_socket_ttl(client, 64);

	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(port);
	remote_addr.sin_addr.s_addr = inet_addr(ip);
	udp_send(client, (vpk_sockaddr*)&remote_addr, msg, strlen(msg));

	return 0;
}

#ifdef USE_ZLOG
#define SAMPLE_ZLOG_FILE_PATH		"."
#define SAMPLE_ZLOG_CONF_FILE		"./zlog.conf"
int sample_zlog_init(int procname)
{
	int rc;

	if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH)) {
		int ret = 0;
		char tmp[256] = {0};
		vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
		printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
		ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
		printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
	}

	rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
	if (rc)	{
		printf("zlog init failed\n");
		return -1;
	}

	LOG_D("hello, zlog");

	return 0;
}
#endif // USE_ZLOG

int main(int argc, char *argv[])
{
	//int ret = 0;

#ifdef USE_ZLOG
	sample_zlog_init(0);
#endif // USE_ZLOG

	int server = 1;
	char* remoteip = "127.0.0.1";
	int rport = 5000;
	int lport = 5002;

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "server",			required_argument,		NULL, 's' },
		{ "remoteip",		required_argument,		NULL, 'i' },
		{ "remoteport",		required_argument,		NULL, 'p' },
		{ "localport",		required_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:s:i:p:l:", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 's':
				server = atoi(optarg);
				break;
			case 'i':
				remoteip = optarg;
				break;
			case 'p':
				rport = atoi(optarg);
				break;
			case 'l':
				lport = atoi(optarg);
				break;
			default:
				break;
		}
	}

	if (server) {
		main_ttl_server(lport); /* ./ttl -s 1 -l 5000 */
	} else {
		main_ttl_client(remoteip, rport, lport); /* ./ttl -s 0 -l 5002 -i 172.17.13.222 -p 5000 */
	}

	return 0;
}
