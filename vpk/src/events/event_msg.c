/**
 * History:
 * ================================================================
 * 2017-12-05 qing.zou created
 *
 */

//#include <time.h>
//#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include "event.h"
#include "event_msg.h"
#include "event_map.h"
#include "event_thread.h"

struct evnotice_table {
	int *notices;
	int total;
};
static struct evnotice_table evtable = {0};

static int evmsg_add(vpk_evbase_t *base, int msgfd, short old, short events, void *p);
static int evmsg_del(vpk_evbase_t *base, int msgfd, short old, short events, void *p);

static const struct eventop evmsg_ops = {
	"notice",
	NULL,
	evmsg_add,
	evmsg_del,
	NULL,
	NULL,
	0
};

#define EVENT_MSG_NOTICE_BUFFER_MAX		1024

#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
static void *evmsg_base_lock = NULL;
static void *evmsg_base_cond = NULL;
#endif

static vpk_evbase_t *evmsg_base = NULL;
static int evmsg_notice_added_cnt = 0;
static int evmsg_base_fd = -1;

static int evmsg_buffer_flag = -1;
static int evmsg_buffer_len = 0;
static char evmsg_buffer[EVENT_MSG_NOTICE_BUFFER_MAX] = {0};

#define EVNOTICEBASE_LOCK()		EVLOCK_LOCK(evmsg_base_lock, 0)
#define EVNOTICEBASE_UNLOCK()	EVLOCK_UNLOCK(evmsg_base_lock, 0)

#define	EVNOTICE_COND_WAIT()		EVTHREAD_COND_WAIT(evmsg_base_cond, evmsg_base_lock)
#define EVNOTICE_COND_BROADCAST()	EVTHREAD_COND_BROADCAST(evmsg_base_cond)

static void evmsg_callback(int fd, short what, void *arg)
{
	static char notices[1024];
	int n, i;
	int ncaught[EVENT_MSG_NOTICE_MAX_NUM];
	vpk_evbase_t *thiz = arg;

	memset(&ncaught, 0, sizeof(ncaught));

	while (1) {
		n = recv(fd, notices, sizeof(notices), 0);
		if (n == -1) {
			if (!(errno == EINTR || errno == EAGAIN))
				EVENT_LOGE("%s: recv error.");
			break;
		} else if (n == 0) {
			break;
		}
		for (i = 0; i < n; ++i) {
			unsigned char key = notices[i];
			EVENT_LOGD(("event msg recv key: %d", (int)key));
			if (key < EVENT_MSG_NOTICE_MAX_NUM)
				ncaught[key]++;
		}
	}

#if 0
	EVBASE_ACQUIRE_LOCK(thiz, th_base_lock);
	for (i = 0; i < EVENT_MSG_NOTICE_MAX_NUM; i++)
	{
		if (ncaught[i])
			evmap_notice_active(thiz, i, ncaught[i]);
	}
	EVBASE_RELEASE_LOCK(thiz, th_base_lock);
#else
	EVNOTICEBASE_LOCK();

	EVBASE_ACQUIRE_LOCK(thiz, th_base_lock);
	for (i = 0; i < EVENT_MSG_NOTICE_MAX_NUM; i++)
	{
		if (ncaught[i]) {
			if (i == evmsg_buffer_flag)
				evmap_notice_active(thiz, i, ncaught[i], evmsg_buffer, evmsg_buffer_len);
			else
				evmap_notice_active(thiz, i, ncaught[i], NULL, 0);
		}
	}
	EVBASE_RELEASE_LOCK(thiz, th_base_lock);

	if (evmsg_buffer_flag > -1) {
		evmsg_buffer_flag = -1;
		EVNOTICE_COND_BROADCAST();
	}

	EVNOTICEBASE_UNLOCK();
#endif
}

int evmsg_init(vpk_evbase_t *base)
{
	base->notice.ev_notice_pair[0] = -1;
	base->notice.ev_notice_pair[1] = -1;
	evmsg_global_setup_locks(1);

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, base->notice.ev_notice_pair) == -1) {
		EVENT_LOGE("%s: socketpair error.", __func__);
		return -1;
	}

	vpk_socket_closeonexec(base->notice.ev_notice_pair[0]);
	vpk_socket_closeonexec(base->notice.ev_notice_pair[1]);

	vpk_socket_nonblocking(base->notice.ev_notice_pair[0]);
	vpk_socket_nonblocking(base->notice.ev_notice_pair[1]);

	vpk_event_assign(&base->notice.ev_notice, base, base->notice.ev_notice_pair[1], 
		VPK_EV_READ | VPK_EV_PERSIST, evmsg_callback, base);

	base->notice.ev_notice.ev_flags |= VPK_EVLIST_INTERNAL;
	base->notice.ev_notice.ev_priority = 0;

	base->evmsgsel = &evmsg_ops;

	return 0;
}

static int evmsg_notice_table_expand(struct evnotice_table *table, int slot)
{
	if (table->total < slot) {
		int total = table->total ? table->total : EVENT_MSG_NOTICE_MAX_NUM;
		int *tmp;

		while (total <= slot) {
			//total <<= 1;
			total = total + (total >> 1);
		}

		tmp = VPK_REALLOC(table->notices, total * sizeof(table->notices[0]));
		if (!tmp)
			return -1;

		memset(&tmp[table->total], 0, (total-table->total) * sizeof(table->notices[0]));

		table->total = total;
		table->notices = tmp;
	}
	return 0;
}

static int evmsg_add(vpk_evbase_t *base, int msgfd, short old, short events, void *p)
{
	struct evmsg_notice *notice = &base->notice;
	return_val_if_fail(msgfd > 0 && msgfd < EVENT_MSG_NOTICE_MAX_NUM, -1);

	EVNOTICEBASE_LOCK();
	if (evmsg_base != base && evmsg_notice_added_cnt) {
		EVENT_LOGW("only one base can have message event notices!");
	}
	evmsg_base = base;
	evmsg_notice_added_cnt = ++notice->ev_notice_added_cnt;
	evmsg_base_fd = base->notice.ev_notice_pair[0];

	if (msgfd >= evtable.total) {
		if (evmsg_notice_table_expand(&evtable, msgfd) == -1)
			goto add_err;
	}
	evtable.notices[msgfd]++;		//... whether could multi-callback for notice
	EVNOTICEBASE_UNLOCK();

	if (!notice->ev_notice_added) {
		if (vpk_event_add(&notice->ev_notice, NULL))
			goto add_err;
		notice->ev_notice_added = 1;
	}

	return 0;
add_err:
	EVNOTICEBASE_LOCK();
	evmsg_notice_added_cnt--;
	notice->ev_notice_added_cnt--;
	EVNOTICEBASE_UNLOCK();
	return -1;
}

static int evmsg_del(vpk_evbase_t *base, int msgfd, short old, short events, void *p)
{
	return_val_if_fail(msgfd > 0 && msgfd < EVENT_MSG_NOTICE_MAX_NUM, -1);

	EVNOTICEBASE_LOCK();
	evmsg_notice_added_cnt--;
	base->notice.ev_notice_added_cnt--;

	if (msgfd < evtable.total)
		evtable.notices[msgfd]--;		//... whether could multi-callback for notice
	EVNOTICEBASE_UNLOCK();

	return 0;
}

//static int evmsg_dispatch(vpk_evbase_t *base, struct timeval *tv)
#if 0
int vpk_evmsg_notice(int key)
{
	int msg = key;
	int flag = 0;

	EVNOTICEBASE_LOCK();
	if (msg < evtable.total)
		flag = evtable.notices[msg];
	EVNOTICEBASE_UNLOCK();

	if (flag > 0) {
		send(evmsg_base_fd, (char*)&msg, 1, 0);
		return 0;
	}

	return -1;
}
#else
int vpk_evmsg_notice(int key, void *data, int size)
{
	int msg = key;
	int flag = 0;
	int len = size > EVENT_MSG_NOTICE_BUFFER_MAX - 1 ? EVENT_MSG_NOTICE_BUFFER_MAX - 1 : size;;

	EVNOTICEBASE_LOCK();
	if (msg < evtable.total)
		flag = evtable.notices[msg];
	EVNOTICEBASE_UNLOCK();

	/* whether this notice is registered */
	if (flag > 0) {
		if (data && len > 0) {
			EVNOTICEBASE_LOCK();
			while (evmsg_buffer_flag > -1)
				EVNOTICE_COND_WAIT();

			/* because of 'msg >= 0' */
			evmsg_buffer_flag = msg;
			evmsg_buffer_len = len;
			memset(evmsg_buffer, 0x00, sizeof(evmsg_buffer));
			memcpy(evmsg_buffer, data, len);

			EVNOTICEBASE_UNLOCK();
		}

		send(evmsg_base_fd, (char*)&msg, 1, 0);
		return 0;
	}

	return -1;
}
#endif

void evmsg_dealloc(vpk_evbase_t *base)
{
	if (base->notice.ev_notice_added) {
		vpk_event_del(&base->notice.ev_notice);
		base->notice.ev_notice_added = 0;
	}

	base->notice.ev_notice.ev_flags = 0;

	EVNOTICEBASE_LOCK();
	if (base == evmsg_base) {
		evmsg_base = NULL;
		evmsg_base_fd = -1;
		evmsg_notice_added_cnt = 0;
	}

	evtable.total = 0;
	if (evtable.notices)
		VPK_FREE(evtable.notices);
	EVNOTICEBASE_UNLOCK();
	
	if (base->notice.ev_notice_pair[0] != -1) {
		close(base->notice.ev_notice_pair[0]);
		base->notice.ev_notice_pair[0] = -1;
	}
	if (base->notice.ev_notice_pair[1] != -1) {
		close(base->notice.ev_notice_pair[1]);
		base->notice.ev_notice_pair[1] = -1;
	}

	evmsg_global_free_locks();
}


#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
int evmsg_global_setup_locks(const int enable_locks)
{
	EVTHREAD_SETUP_GLOBAL_LOCK(evmsg_base_lock, 0);
	//EVTHREAD_ALLOC_LOCK(evmsg_base_lock, 0);
	EVTHREAD_ALLOC_COND(evmsg_base_cond);
	return 0;
}
void evmsg_global_free_locks(void)
{
	EVTHREAD_FREE_COND(evmsg_base_cond);
	EVTHREAD_FREE_LOCK(evmsg_base_lock, 0);
}
#endif
