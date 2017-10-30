/**
 * History:
 * ================================================================
 * 2017-08-03 qing.zou created
 *
 */
#ifndef TIMER_EVENT_H
#define TIMER_EVENT_H

//#include <sys/queue.h>		/* tailq */

#include "vpk_typedef.h"
#include "vpk_logging.h"
//#include "vpk_minheap.h"

TIMA_BEGIN_DELS

//#define TIMER_LOGD  LOG_D
//#define TIMER_LOGI  LOG_I
//#define TIMER_LOGW  LOG_W
//#define TIMER_LOGE  LOG_E
//#define TIMER_LOGF  LOG_F
#if 1
#define TIMER_LOGD(x)  LOG_D x
#define TIMER_LOGI  LOG_I
#define TIMER_LOGW  LOG_W
#define TIMER_LOGE  LOG_E
#define TIMER_LOGF  LOG_F
#else
#define TIMER_LOGD(x)
#define TIMER_LOGI(x)
#define TIMER_LOGW  LOG_W
#define TIMER_LOGE  LOG_E
#define TIMER_LOGF  LOG_F
#endif

//#define TIMER_LOGD(format, args...) LOG_D(format, ##args)
//#define TIMER_LOGI(format, args...) LOG_I(format, ##args)
//#define TIMER_LOGW(format, args...) LOG_W(format, ##args)
//#define TIMER_LOGE(format, args...) LOG_E(format, ##args)
//#define TIMER_LOGF(format, args...) LOG_F(format, ##args)

#if 0
#define VPK_HAVE_CLOCK_GETTIME
#define CLOCK_MONOTONIC

/** Indicates that a timeout has occurred. */
#define VPK_EV_TIMEOUT			0x01
/** Wait for FD to become readable */
#define VPK_EV_READ				0x02
/** Wait for FD to become writable */
#define VPK_EV_WRITE			0x04
/** Wait for a POSIX signal to be raised */
#define VPK_EV_SIGNAL			0x08
/**
 * Persistent event: won't get removed automatically when activated.
 *
 * When a persistent event with a timeout becomes activated, its timeout
 * is reset to 0.
 */
#define VPK_EV_PERSIST			0x10
/** Select edge-triggered behavior, if supported by the backend. */
#define VPK_EV_ET				0x20

typedef void (*vpk_event_callback)(int fd, short events, void *args);

#define ev_io_timeout	_ev.ev_io.ev_timeout

#define VPK_EVLIST_TIMEOUT		0x01
#define VPK_EVLIST_INSERTED		0x02
#define VPK_EVLIST_SIGNAL		0x04
#define VPK_EVLIST_ACTIVE		0x08
#define VPK_EVLIST_INTERNAL		0x10
#define VPK_EVLIST_INIT			0x80

/* Possible values for ev_closure in struct event. */
#define VPK_EV_CLOSURE_NONE			0
#define VPK_EV_CLOSURE_SIGNAL		1
#define VPK_EV_CLOSURE_PERSIST		2

TAILQ_HEAD(vpk_event_queue, vpk_events);



typedef struct vpk_timer_t
{
	/** Number of total events added to this timer */
	int event_count;
	/** Number of total events active in this timer */
	int event_count_active;

	int running_loop;

	struct vpk_event_queue *activequeues;
	int						nactivequeues;

	struct timeval			timer_tv;
	vpk_minheap_t			timeheap;

#if defined(VPK_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	/** 
	 * Difference between internal time (maybe from clock_gettime) and
	 * gettimeofday. 
	 */
	struct timeval			tv_clock_diff;
	/** Second in which we last updated tv_clock_diff, in monotonic time. **/
	time_t					last_updated_clock_diff;
#endif
} vpk_timer_t;

#endif

TIMA_END_DELS

#endif // TIMER_EVENT_H
