/**
 * History:
 * ================================================================
 * 2017-04-11 qing.zou created
 *
 */
#ifndef VPK_EVENT_H
#define VPK_EVENT_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

#define EVENT_MQ_MSG_NUM_MAX	10
#define EVENT_MQ_MSG_LEN_MAX	16


//enum {
//// #define KEY_EVENT(NAME, TYPE, FIELD, VALUE, READABLE)	VPK_KEY_EVENT_ ## NAME,
//#define KEY_EVENT(NAME, TYPE, FIELD, VALUE, READABLE)	NAME,
//// KEY_EVENT(VPK_KEY_EVENT_SYS_ACC_ON, VPK_EVENT_NOTICE, EVENT_SYS_ACC_ON, 0x1001, "acc on")
//#include "macro_event.h"
//#undef KEY_EVENT
//};

#define    extends_event()     unsigned int  type

typedef struct vpk_abnormal_t
{
	extends_event();
	int keycode;
}vpk_abnormal_t;

typedef struct vpk_alert_t
{
	extends_event();
	int keycode;
}vpk_alert_t;

typedef struct vpk_notice_t
{
	extends_event();
	int keycode;
}vpk_notice_t;

typedef union vpk_event_t
{
	extends_event();
	vpk_alert_t		alert;
	vpk_abnormal_t	abnormal;
	vpk_notice_t	notice;
}vpk_event_t;

struct vpk_eventq_t;
typedef struct vpk_eventq_t vpk_eventq_t;

/**
 * Create and open a event queue object.
 *
 * @param name		The event queue name which was just "/" followed by some characters..
 * @param mode		r/w/a+
 *
 * @return The event queue object.
 */
VPKAPI vpk_eventq_t* vpk_eventq_open(const char* name, const char* mode);

/**
 * Receive event(alert, and so on) from event queue.
 *
 * @param queue		The event queue object.
 * @param e			an event
 *
 * @return The receive message len, -1 on failure.
 */
VPKAPI int vpk_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e);

/**
 * Post an event(alert, and so on) to event queue.
 *
 * @param queue		The event queue object.
 * @param e			an event
 *
 * @return 0 on success, -1 on failure.
 */
VPKAPI int vpk_eventq_post(vpk_eventq_t *queue, vpk_event_t* e);

/**
 * Close event queue.
 *
 * @param queue		The event queue object.
 *
 * @return 0 on success.
 */
VPKAPI int vpk_eventq_close(vpk_eventq_t *queue);

/**
 * Destroy event queue.
 *
 * @param queue		The event queue object.
 *
 * @return 0 on success.
 */
VPKAPI int vpk_eventq_destroy(vpk_eventq_t *queue);


TIMA_END_DELS

#endif //VPK_EVENT_H
