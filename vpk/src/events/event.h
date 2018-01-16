/**
 * History:
 * ================================================================
 * 2017-08-03 qing.zou created
 *
 */
#ifndef TIMER_EVENT_H
#define TIMER_EVENT_H

//#include <sys/queue.h>		/* tailq */

#include "vpk_events.h"
#include "vpk_typedef.h"
#include "vpk_logging.h"
#include "vpk_util.h"
//#include "vpk_minheap.h"

#include "event_msg.h"

TIMA_BEGIN_DELS

#if 0
#define EVENT_LOGD(x)	LOG_D x
#define EVENT_LOGI		LOG_I
#define EVENT_LOGW		LOG_W
#define EVENT_LOGE		LOG_E
#define EVENT_LOGF		LOG_F
#else
#define EVENT_LOGD(x)
#define EVENT_LOGI		LOG_I
#define EVENT_LOGW		LOG_W
#define EVENT_LOGE		LOG_E
#define EVENT_LOGF		LOG_F
#endif

#define ev_notice_next	_ev.ev_notice.ev_notice_next
#define ev_io_next	_ev.ev_io.ev_io_next
#define ev_io_timeout	_ev.ev_io.ev_timeout

#define ev_ncalls	_ev.ev_notice.ev_ncalls
#define ev_pdata	_ev.ev_notice.ev_pdata


#define VPK_EVLIST_TIMEOUT			0x01
#define VPK_EVLIST_INSERTED			0x02
#define VPK_EVLIST_SIGNAL			0x04
#define VPK_EVLIST_ACTIVE			0x08
#define VPK_EVLIST_INTERNAL			0x10
#define VPK_EVLIST_NOTICE			0x40		// notice
#define VPK_EVLIST_INIT				0x80

/* Possible values for ev_closure in struct event. */
#define VPK_EV_CLOSURE_NONE			0
#define VPK_EV_CLOSURE_SIGNAL		1
#define VPK_EV_CLOSURE_PERSIST		2
#define VPK_EV_CLOSURE_NOTICE		3

TAILQ_HEAD(vpk_event_queue, vpk_events);

struct event_io_map {
	/* An array of evmap_io * or of evmap_signal *; empty entries are
	 * set to NULL. */
	void **entries;
	/* The number of entries available in entries */
	int nentries;
};
#define event_notice_map event_io_map

struct event_change {
	/* The fd whose events are to be changed */
	int fd;
	/* The events that were enabled on the fd before any of these changes
	   were made.  May include VPK_EV_READ or VPK_EV_WRITE. */
	short old_events;

	/* The changes that we want to make in reading and writing on this fd.
	 * If this is a signal, then read_change has EV_CHANGE_SIGNAL set,
	 * and write_change is unused. */
	unsigned char read_change;
	unsigned char write_change;
};

/* If set, add the event. */
#define VPK_EVCHANGE_ADD		0x01
/* If set, delete the event.  Exclusive with VPK_EVCHANGE_ADD */
#define VPK_EVCHANGE_DEL		0x02
/* If set, this event refers a signal, not an fd. */
#define VPK_EVCHANGE_SIGNAL		VPK_EV_SIGNAL
/* Set for persistent events.  Currently not used. */
#define VPK_EVCHANGE_PERSIST	VPK_EV_PERSIST
/* Set for adding edge-triggered events. */
#define VPK_EVCHANGE_ET			VPK_EV_ET
/* If set, this event refers a notice msg, not an fd. */
#define VPK_EVCHANGE_NOTICE		VPK_EV_NOTICE

struct eventop {
	const char *name;
	void *(*init)(vpk_evbase_t *);
	/** 
	 * Enable reading/writing on a given fd or signal.  'events' will be
	 * the events that we're trying to enable: one or more of EV_READ,
	 * EV_WRITE, EV_SIGNAL, and EV_ET.  'old' will be those events that
	 * were enabled on this fd previously.  'fdinfo' will be a structure
	 * associated with the fd by the evmap; its size is defined by the
	 * fdinfo field below.  It will be set to 0 the first time the fd is
	 * added.  The function should return 0 on success and -1 on error.
	 */
	int (*add)(vpk_evbase_t *, int fd, short old, short events, void *fdinfo);
	/** As "add", except 'events' contains the events we mean to disable. */
	int (*del)(vpk_evbase_t *, int fd, short old, short events, void *fdinfo);
	int (*dispatch)(vpk_evbase_t *, struct timeval *);
	/** Function to clean up and free our data from the event_base. */
	void (*dealloc)(vpk_evbase_t *);

	//int need_reinit;
	//enum event_method_feature features;
	size_t fdinfo_len;
};

struct vpk_evbase_t
{
	/** multi-plexing **/
	const struct eventop	*evsel;
	void					*priv;

	/** uses for message event notice */
	const struct eventop	*evmsgsel;
	struct evmsg_notice		notice;

	/** Number of total events added to this base */
	int						event_count;
	/** Number of total events active in this base */
	int						event_count_active;

	int						running_loop;

	struct vpk_event_queue	*activequeues;
	int						nactivequeues;

	struct event_io_map		iomap;
	struct event_notice_map noticemap;

	struct vpk_event_queue	eventqueue;

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

	unsigned long			th_owner_id;
	void					*th_base_lock;
	vpk_events				*current_event;
	void					*current_event_cond;
	int						current_event_waiters;

	int						is_notify_pending;
	int						th_notify_fd[2];
	vpk_events				th_notify;
	int						(*th_notify_func)(vpk_evbase_t *base);
};

int event_thread_make_notifiable(vpk_evbase_t *thiz);

int vpk_event_data_set(vpk_events *ev, void *data, int len);

void event_active_nolock(vpk_events *ev, int flag, short ncalls);

TIMA_END_DELS

#endif // TIMER_EVENT_H
