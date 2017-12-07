/**
 * History:
 * ================================================================
 * 2017-08-03 qing.zou created
 *
 */
#ifndef VPK_TIMER_H
#define VPK_TIMER_H

#include <sys/queue.h>		/* tailq */

#include "vpk_typedef.h"
#include "vpk_minheap.h"

TIMA_BEGIN_DELS

#define VPK_HAVE_CLOCK_GETTIME
//#define CLOCK_MONOTONIC

/** Indicates that a timeout has occurred. */
#define VPK_EV_TIMEOUT			0x01
/** Wait for FD to become readable */
#define VPK_EV_READ				0x02
/** Wait for FD to become writable */
#define VPK_EV_WRITE			0x04
/** Wait for a POSIX signal to be raised */
#define VPK_EV_SIGNAL			0x08
/** Wait for a custom notice to be raised */
#define VPK_EV_NOTICE			0x40
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

#define ev_notice_next	_ev.ev_notice.ev_notice_next
#define ev_io_next	_ev.ev_io.ev_io_next
#define ev_io_timeout	_ev.ev_io.ev_timeout

#define ev_ncalls	_ev.ev_notice.ev_ncalls

#define VPK_EVLIST_TIMEOUT		0x01
#define VPK_EVLIST_INSERTED		0x02
#define VPK_EVLIST_SIGNAL		0x04
#define VPK_EVLIST_ACTIVE		0x08
#define VPK_EVLIST_INTERNAL		0x10
#define VPK_EVLIST_NOTICE		0x40		// notice
#define VPK_EVLIST_INIT			0x80

/* Possible values for ev_closure in struct event. */
#define VPK_EV_CLOSURE_NONE			0
#define VPK_EV_CLOSURE_SIGNAL		1
#define VPK_EV_CLOSURE_PERSIST		2
#define VPK_EV_CLOSURE_NOTICE		3

TAILQ_HEAD(vpk_event_queue, vpk_events);

struct vpk_evbase_t;
typedef struct vpk_evbase_t vpk_evbase_t;


VPKAPI int vpk_event_assign(vpk_events *ev, vpk_evbase_t *base, int fd, short events, vpk_event_callback callback, void *arg);
VPKAPI int vpk_event_add(vpk_events *ev, const struct timeval *tv);
VPKAPI int vpk_event_del(vpk_events *ev);
VPKAPI vpk_events *vpk_event_new(vpk_evbase_t *base, int fd, short events, vpk_event_callback callback, void *arg);
VPKAPI void vpk_event_free(vpk_events *ev);
VPKAPI vpk_evbase_t* vpk_evbase_create(void);
VPKAPI int vpk_evbase_loop(vpk_evbase_t* thiz, int flags);
VPKAPI void vpk_evbase_destroy(vpk_evbase_t* thiz);

VPKAPI int vpk_evmsg_notice(int key);

TIMA_END_DELS

#endif // VPK_TIMER_H
