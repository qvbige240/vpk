/**
 * History:
 * ================================================================
 * 2017-04-11 qing.zou created
 *
 */
#ifndef VPK_IEVENTQ_H
#define VPK_IEVENTQ_H

#include "vpk_event.h"
#include "vpk_typedef.h"
#include "vpk_object.h"

TIMA_BEGIN_DELS


typedef struct eventq_ops_s
{
	vpk_eventq_t*	(*init)(void *ctx);
	int				(*empty)(const vpk_eventq_t* queue);
	int				(*recv)(vpk_eventq_t *queue, vpk_event_t* e);
	int				(*post)(vpk_eventq_t *queue, vpk_event_t* e);
	int				(*close)(vpk_eventq_t *queue);
}eventq_ops;

#define extends_eventq()						\
	extends_object();							\
	int fd; 									\
	char name[32];								\
	char recv_buff[EVENT_MQ_MSG_LEN_MAX];		\
	const eventq_ops* e_ops

struct vpk_eventq_t
{
	extends_eventq();
};

int vpk_eventq_init(void);
// vpk_eventq_t* vpk_eventq(void);

TIMA_END_DELS

#endif //VPK_IEVENTQ_H
