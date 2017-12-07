/**
 * History:
 * ================================================================
 * 2017-12-05 qing.zou created
 *
 */

#ifndef EVENT_MSG_H
#define EVENT_MSG_H

//#include <sys/queue.h>		/* tailq */

#include "vpk_typedef.h"
#include "vpk_logging.h"
#include "vpk_minheap.h"

TIMA_BEGIN_DELS

// #define EVENT_MSG_NOTICE_MAX_NUM	VPK_KEY_EVENT_MAX_NUM
#define EVENT_MSG_NOTICE_MAX_NUM	10


struct evmsg_notice {
	vpk_events	ev_notice;
	int			ev_notice_pair[2];		/* socketpair used to send notifications */
	int			ev_notice_added;
	int			ev_notice_added_cnt;
};

int vpk_evmsg_notice(int key);

int evmsg_init(vpk_evbase_t *base);
void evmsg_dealloc(vpk_evbase_t *base);

#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
int evmsg_global_setup_locks(const int enable_locks);
void evmsg_global_free_locks(void);
#endif

TIMA_END_DELS

#endif // EVENT_MSG_H
