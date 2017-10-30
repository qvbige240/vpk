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
#include <sys/time.h>

#include "vpk.h"

#if defined(_X86_)
#else
#endif

#include <errno.h>

static int use_monotonic;
static void detect_monotonic(void)
{
	struct timespec	ts;
	static int use_monotonic_initialized = 0;

	if (use_monotonic_initialized)
		return;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		use_monotonic = 1;
	LOG_D("ts: %d %ld", ts.tv_sec, ts.tv_nsec);

	use_monotonic_initialized = 1;
}

#define CLOCK_SYNC_INTERVAL -1
static time_t last_updated_clock_diff = 0;
static struct timeval tv_clock_diff;
static int gettime(struct timeval *tp)
{
	//if (base->tv_cache.tv_sec) {
	//	*tp = base->tv_cache;
	//	return (0);
	//}

	if (use_monotonic) {
		struct timespec	ts;

		if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
			return (-1);

		tp->tv_sec = ts.tv_sec;
		tp->tv_usec = ts.tv_nsec / 1000;
		if (last_updated_clock_diff + CLOCK_SYNC_INTERVAL < ts.tv_sec) {
				struct timeval tv;
				vpk_gettimeofday(&tv,NULL);
				vpk_timersub(&tv, tp, &tv_clock_diff);
				last_updated_clock_diff = ts.tv_sec;
				LOG_D("real time: %d %d, clock mono: %d %ld, diff: %d %d",
					tv.tv_sec, tv.tv_usec, ts.tv_sec, ts.tv_nsec, tv_clock_diff.tv_sec, tv_clock_diff.tv_usec);
		}

		return (0);
	}

	return (vpk_gettimeofday(tp, NULL));
}


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
#define MAX_SECONDS_IN_MSEC_LONG \
	(((LONG_MAX) - 999) / 1000)

long tv_to_msec(const struct timeval *tv)
{
	if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LONG)
		return -1;

	return (tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000);
}

int make_socket_closeonexec(int fd)
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

static int epoll_test(struct timeval *tv)
{
	int fd;
	if ((fd = epoll_create(32000)) == -1) {
		if (errno != ENOSYS)
			LOG_E("epoll_create");
		return -1;
	}

	make_socket_closeonexec(fd);
	struct epoll_event *events = malloc(32 * sizeof(struct epoll_event));
	if (events == NULL) {
		close(fd);
		return -1;
	}

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

	//for (i = 0; i < res; i++) {
	//	int what = events[i].events;
	//	short ev = 0;

	//	if (what & (EPOLLHUP|EPOLLERR)) {
	//		ev = EV_READ | EV_WRITE;
	//	} else {
	//		if (what & EPOLLIN)
	//			ev |= EV_READ;
	//		if (what & EPOLLOUT)
	//			ev |= EV_WRITE;
	//	}

	//	if (!ev)
	//		continue;

	//	evmap_io_active(base, events[i].data.fd, ev | EV_ET);
	//}

	free(events);
	close(fd);

	return 0;
}

void time_test()
{
	time_t timestamp;
	struct tm *locals;
	//char timestr[] = "2018-11-01 11:30:00";

	timestamp = time(NULL);
	LOG_D("timestamp: %ld", timestamp);
	LOG_D("ctime: %s", ctime(&timestamp));
	locals = localtime(&timestamp);
	LOG_D("%d-%d-%d %02d:%02d:%02d", 
		locals->tm_year+1900, locals->tm_mon+1, locals->tm_mday, 
		locals->tm_hour, locals->tm_min, locals->tm_sec);

	timestamp = mktime(locals);
	LOG_D("timestamp: %ld\n", timestamp);
}

#if 1
static void set_timer(int seconds, int mseconds)
{
	struct timeval temp;

	temp.tv_sec = seconds;
	temp.tv_usec = mseconds;

	select(0, NULL, NULL, NULL, &temp);
	printf("timer\n");

	return ;
}
#elif 0
#elif 0
#endif

#include "vpk_timer.h"

static struct timeval prev;
static void task_heart_beat(int fd, short event, void *arg)
{
	//int value = HEART_BEAT_MESSAGE_VALUE;
	double elapsed;
	struct timeval nowtime, difference;

	vpk_gettimeofday(&nowtime, NULL);
	vpk_timersub(&nowtime, &prev, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);
	prev = nowtime;

	LOG_D("task_heart_beat, at %d: %.6f seconds elapsed.", nowtime.tv_sec, elapsed);
}

static int timer_test(void)
{
	vpk_events task_event;
	vpk_timer_t* base = vpk_timer_create();

	int flags = VPK_EV_PERSIST;
	//flags = 0;

	vpk_event_assign(&task_event, base, 0, flags, task_heart_beat, NULL);

	struct timeval tv;
	vpk_timerclear(&tv);
	tv.tv_sec = 5;
	vpk_gettimeofday(&prev, NULL);
	vpk_timer_event_add(&task_event, &tv);

	vpk_timer_loop(base, 0);

	return 0;
}

int timer_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	//char* pathname = "./ipc";
	//int proj_id = 0x10;
	char* type = "time";
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

	time_test();

	detect_monotonic();
	struct timeval now;
	gettime(&now);
	LOG_D("now: %d %d\n", now.tv_sec, now.tv_usec);

	gettimeofday(&prev, 0);

	if (strcasecmp(type, "select") == 0) {
		set_timer(5, 10);
	} else if (strcasecmp(type, "epoll") == 0) {
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 500000;
		epoll_test(&tv);
	} else {
		timer_test();
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);		//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
