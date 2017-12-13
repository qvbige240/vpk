/**
 * History:
 * ================================================================
 * 2017-05-28 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

#include <sys/eventfd.h>


#include "vpk.h"
#include "vpk_events.h"

#include <errno.h>


#include <fcntl.h>
#include <limits.h>
//#include <sys/types.h>
#include <sys/epoll.h>
/* On Linux kernels at least up to 2.6.24.4, epoll can't handle timeout
 * values bigger than (LONG_MAX - 999ULL)/HZ.  HZ in the wild can be
 * as big as 1000, and LONG_MAX can be as small as (1<<31)-1, so the
 * largest number of msec we can support here is 2147482.  Let's
 * round that down by 47 seconds.
 */

#define MAX_EPOLL_TIMEOUT_MSEC (35*60*1000)
#define MAX_SECONDS_IN_MSEC_LONG	(((LONG_MAX) - 999) / 1000)

static long tv_to_msec(const struct timeval *tv)
{
	if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LONG)
		return -1;

	return (tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000);
}

static int make_socket_closeonexec(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFD, NULL)) < 0) {
		LOG_E("fcntl(%d, F_GETFD)", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
		LOG_E("fcntl(%d, F_SETFD)", fd);
		return -1;
	}

	return 0;
}

struct epoll_priv {
	struct epoll_event *events;
	int nevents;
	int epfd;
};

struct event_evbase {
	
	int th_notify_fd[2];
};

static void notify_read_test_eventfd(int fd, short what, void *arg);
static void *epinit_test(void)
{
	int fd;
	struct epoll_priv *epriv;
	if ((fd = epoll_create(32000)) == -1) {
		if (errno != ENOSYS)
			LOG_E("epoll_create");
		return NULL;
	}

	make_socket_closeonexec(fd);
	epriv = malloc(sizeof(struct epoll_priv));
	if (epriv == NULL) {
		close(fd);
		return NULL;
	}
	epriv->epfd = fd;
	epriv->events = malloc(32 * sizeof(struct epoll_event));
	if (epriv->events == NULL) {
		free(epriv);
		close(fd);
		return NULL;
	}
	epriv->nevents = 32;

	return epriv;
}

static int epwait_test(struct epoll_priv *priv, struct timeval *tv)
{
	int i;
	int fd = priv->epfd;
	struct epoll_event *events = priv->events;

	int res;
	long timeout = -1;
	if (tv != NULL) {
		timeout = tv_to_msec(tv);
		if (timeout < 0 || timeout > MAX_EPOLL_TIMEOUT_MSEC) {
			/* Linux kernels can wait forever if the timeout is
			 * too big; see comment on MAX_EPOLL_TIMEOUT_MSEC. */
			timeout = MAX_EPOLL_TIMEOUT_MSEC;
		}
	}
	//unlock
	LOG_D("epoll_wait %ld(ms)", timeout);
	res = epoll_wait(fd, events, 32, timeout);

	//EVBASE_ACQUIRE_LOCK(base, th_base_lock);

	if (res == -1) {
		if (errno != EINTR) {
			LOG_E("epoll_wait");
			return (-1);
		}

		return (0);
	}

	for (i = 0; i < res; i++) {
		int what = events[i].events;
		short ev = 0;

		if (what & (EPOLLHUP|EPOLLERR)) {
			ev = VPK_EV_READ | VPK_EV_WRITE;
		} else {
			if (what & EPOLLIN)
				ev |= VPK_EV_READ;
			if (what & EPOLLOUT)
				ev |= VPK_EV_WRITE;
		}

		if (!ev)
			continue;

		//evmap_io_active(base, events[i].data.fd, ev | EV_ET);
		LOG_I("fd = %d, ev = 0x%02x", events[i].data.fd, ev);
		notify_read_test_eventfd(events[i].data.fd, ev, NULL);
	}

	free(events);
	close(fd);

	return 0;
}

static int epapply_change_test(int epfd, int fd)
{
	struct epoll_event epev;
	int op, events = 0;

	events = 0;
	op = EPOLL_CTL_ADD;
	events |= EPOLLIN;

	memset(&epev, 0, sizeof(epev));
	epev.data.fd = fd;
	epev.events = events;

	if (epoll_ctl(epfd, op, fd, &epev) == -1) {
		LOG_E("Epoll MOD(%d) on %d retried as ADD; that failed too",
			(int)epev.events, fd);
		return -1;
	} else {
		LOG_D("Epoll MOD(%d) on %d retried as ADD; succeeded.",
			(int)epev.events, fd);
	}

	return 0;
}

static int wait_test(int read_fd)
{
	struct epoll_priv *priv = epinit_test();
	epapply_change_test(priv->epfd, read_fd);
	epwait_test(priv, NULL);

	return 0;
}

static int notify_write_test_eventfd(struct event_evbase *base)
{
	uint64_t msg = 1;
	int r;
	do {
		r = write(base->th_notify_fd[0], (void*) &msg, sizeof(msg));
	} while (r < 0 && errno == EAGAIN);

	LOG_D("notify write msg, ret = %d", r);
	return (r < 0) ? -1 : 0;
}

static void notify_read_test_eventfd(int fd, short what, void *arg)
{
	uint64_t msg;
	ssize_t r;
	//struct event_base *base = arg;

	r = read(fd, (void*) &msg, sizeof(msg));
	if (r<0 && errno != EAGAIN) {
		LOG_E("Error reading from eventfd");
		return;
	}
	LOG_D("read: %d", msg);
	//EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	//base->is_notify_pending = 0;
	//EVBASE_RELEASE_LOCK(base, th_base_lock);
}

#if 1
static int evthread_event_notifiable(struct event_evbase *base)
{
	base->th_notify_fd[0] = eventfd(0, EFD_CLOEXEC);
	if (base->th_notify_fd[0] >= 0) {
		make_socket_closeonexec(base->th_notify_fd[0]);
	}
	return 0;
}
#elif 0
#elif 0
#endif

struct event_evbase* base;
static void *epoll_event_test(void* arg)
{
	struct event_evbase* eb = (struct event_evbase *)arg;

	//base = malloc(sizeof(struct event_evbase));
	//struct event_evbase* eb = base;
	if (eb)
	{
		LOG_D("[r]thread id = %lu", pthread_self());
		evthread_event_notifiable(eb);
		wait_test(eb->th_notify_fd[0]);	//read fd
	}

	return 0;
}

static void *evthread_notify_test(void *arg)
{
	char ch;
	scanf("%c", &ch);
	LOG_D("ch = %c", ch);

	LOG_D("[w]thread id = %lu", pthread_self());
	notify_write_test_eventfd(arg);
	return 0;
}

static int epoll_test(void)
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test1, pth_test2;

	base = malloc(sizeof(struct event_evbase));
 	ret = pthread_create(&pth_test1, NULL, epoll_event_test, (void*)base);
 	if (ret != 0)
 		LOG_E("create thread \'epoll_test\' failed");

	ret = pthread_create(&pth_test2, NULL, evthread_notify_test, (void*)base);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test1, &thread_result);
	pthread_join(pth_test2, &thread_result);

	return 0;
}


int epoll_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	//char* pathname = "./ipc";
	//int proj_id = 0x10;
	char* type = "epoll";
	//if (argc > 1)
	//{
	//	type = argv[1];
	//}

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

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	if (strcasecmp(type, "select") == 0) {

	} else if (strcasecmp(type, "epoll") == 0) {
		//struct timeval tv;
		//tv.tv_sec = 5;
		//tv.tv_usec = 500000;
		epoll_test();
	} else {

	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);		//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
