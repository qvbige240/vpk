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
#define EVENT_MQ_MSG_LEN_MAX	32

#define    extends_event()     unsigned int  type

typedef struct vpk_alert_t
{
	extends_event();
	int keycode;
}vpk_alert_t;

typedef union vpk_event_t
{
	extends_event();
	vpk_alert_t		alert;
}vpk_event_t;

struct vpk_eventq_t;
typedef struct vpk_eventq_t vpk_eventq_t;

VPKAPI vpk_eventq_t* vpk_eventq_open(void);
VPKAPI int vpk_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e);
VPKAPI int vpk_eventq_post(vpk_eventq_t *queue, vpk_event_t* e);
VPKAPI int vpk_eventq_close(vpk_eventq_t *queue);
VPKAPI int vpk_eventq_destroy(vpk_eventq_t *queue);


TIMA_END_DELS

#endif //VPK_EVENT_H
