/**
 * History:
 * ================================================================
 * 2017-08-03 qing.zou created
 *
 */

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/eventfd.h>

#include "event.h"
#include "event_map.h"
#include "event_thread.h"

extern const struct eventop epollops;

static const struct eventop *eventops[] = {
	&epollops,
	NULL
};

static int event_thread_notify(vpk_evbase_t *thiz);

static int use_monotonic = 0;
static void detect_monotonic(void)
{
#if defined(VPK_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	struct timespec	ts;
	static int use_monotonic_initialized = 0;

	if (use_monotonic_initialized)
		return;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		use_monotonic = 1;
	EVENT_LOGD(("mono ts: %d %ld", ts.tv_sec, ts.tv_nsec));

	use_monotonic_initialized = 1;
#endif
}

#define CLOCK_SYNC_INTERVAL -1

static int vpk_gettime(vpk_evbase_t* base, struct timeval *tp)
{
#if defined(VPK_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	if (use_monotonic) {
		struct timespec	ts;

		if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
			return (-1);

		tp->tv_sec = ts.tv_sec;
		tp->tv_usec = ts.tv_nsec / 1000;
		if (base->last_updated_clock_diff + CLOCK_SYNC_INTERVAL < ts.tv_sec) {
			struct timeval tv;
			vpk_gettimeofday(&tv,NULL);
			vpk_timersub(&tv, tp, &base->tv_clock_diff);
			base->last_updated_clock_diff = ts.tv_sec;
			//EVENT_LOGD(("real time: %d %d, clock mono: %d %ld, diff: %d %d",
			//	tv.tv_sec, tv.tv_usec, 
			//	ts.tv_sec, ts.tv_nsec, 
			//	base->tv_clock_diff.tv_sec, base->tv_clock_diff.tv_usec));
		}

		return (0);
	}
#endif

	return (vpk_gettimeofday(tp, NULL));
}

static void event_queue_insert(vpk_evbase_t* base, vpk_events* ev, int queue)
{
	if ((ev->ev_flags & queue)) {
		if (queue & VPK_EVLIST_ACTIVE)
			return;
		EVENT_LOGE("event(%p) already on queue %x", ev, queue);
		return;
	}

	if (~ev->ev_flags & VPK_EVLIST_INTERNAL)
		base->event_count++;

	ev->ev_flags |= queue;
	switch (queue) {
		case VPK_EVLIST_INSERTED:
			TAILQ_INSERT_TAIL(&base->eventqueue, ev, ev_next);
			break;
		case VPK_EVLIST_ACTIVE:
			base->event_count_active++;
			TAILQ_INSERT_TAIL(&base->activequeues[ev->ev_priority], ev, ev_active_next);
			break;
		case VPK_EVLIST_TIMEOUT:
			minheap_push(&base->timeheap, ev);
			break;
		default:
			EVENT_LOGE("unknown queue %x", queue);
			break;
	}
}

static void event_queue_remove(vpk_evbase_t* base, vpk_events* ev, int queue)
{
	if (!(ev->ev_flags & queue)) {
		EVENT_LOGE("event(%p) not on queue %x", ev, queue);
		return;
	}

	if (~ev->ev_flags & VPK_EVLIST_INTERNAL)
		base->event_count--;

	ev->ev_flags &= ~queue;
	switch (queue) {
		case VPK_EVLIST_INSERTED:
			TAILQ_REMOVE(&base->eventqueue, ev, ev_next);
			break;
		case VPK_EVLIST_ACTIVE:
			base->event_count_active--;
			TAILQ_REMOVE(&base->activequeues[ev->ev_priority], ev, ev_active_next);
			break;
		case VPK_EVLIST_TIMEOUT:
			minheap_erase(&base->timeheap, ev);
			break;
		default:
			EVENT_LOGE("unknown queue %x", queue);
			break;
	}
}

void event_active_nolock(vpk_events *ev, int flag, short ncalls)
{
	vpk_evbase_t *thiz;

	EVENT_LOGD(("event_active: %p (fd %d), flag 0x%02x, callback %p", ev, (ev->ev_fd), (int)flag, ev->event_callback));

	if (ev->ev_flags & VPK_EVLIST_ACTIVE) {
		ev->ev_result |= flag;
		EVENT_LOGW("event already be in active!");
		return;
	}

	thiz = ev->ev_base;
	if (thiz == NULL) {
		EVENT_LOGW("ev->ev_base is NULL!");
		return;
	}
	ev->ev_result = flag;

	if (ev->ev_events & (VPK_EV_SIGNAL|VPK_EV_NOTICE)) {
		if (thiz->current_event == ev && !EVBASE_IN_THREAD(thiz)) {
			++thiz->current_event_waiters;
			EVTHREAD_COND_WAIT(thiz->current_event_cond, thiz->th_base_lock);
		}
		ev->ev_ncalls = ncalls;
		//ev->ev_pncalls = NULL;	//...
	}

	event_queue_insert(thiz, ev, VPK_EVLIST_ACTIVE);

	if (EVBASE_NEED_NOTIFY(thiz))
		event_thread_notify(thiz);
}

static int event_priority_init(vpk_evbase_t* base, int npriorities)
{
	int i = 0;
	if (base->event_count_active || npriorities < 1 || npriorities >= 256)
		return -1;

	if (npriorities == base->nactivequeues)
		return 0;

	if (base->nactivequeues) {
		VPK_FREE(base->activequeues);
		base->nactivequeues = 0;
	}

	base->activequeues = (struct vpk_event_queue *)VPK_CALLOC(npriorities, sizeof(struct vpk_event_queue));
	if (base->activequeues == NULL) {
		EVENT_LOGE("calloc failed!");
		return -1;
	}
	base->nactivequeues = npriorities;

	for (i = 0; i < base->nactivequeues; i++)	{
		TAILQ_INIT(&base->activequeues[i]);
	}

	return 0;
}

static int event_add_internal(vpk_events* ev, const struct timeval* tv, int tv_is_absolute)
{
	int notify = 0, ret = 0;
	vpk_evbase_t* thiz = ev->ev_base;

	EVENT_LOGD(("event_add: event: %p (fd %d), %s%s%s call %p",
		ev,
		ev->ev_fd,
		ev->ev_events & VPK_EV_READ ? "VPK_EV_READ " : " ",
		ev->ev_events & VPK_EV_WRITE ? "VPK_EV_WRITE " : " ",
		tv ? "VPK_EV_TIMEOUT " : " ",
		ev->event_callback));
	
	/*
	 * prepare for timeout insertion further below, if we get a
	 * failure on any step, we should not change any state.
	 */
	if (tv != NULL && !(ev->ev_flags & VPK_EVLIST_TIMEOUT)) {
		if (minheap_reserve(&thiz->timeheap, 1 + minheap_size(&thiz->timeheap)) == -1)
			return (-1);
	}

#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
	if (thiz->current_event == ev && (ev->ev_events & (VPK_EV_NOTICE|VPK_EV_SIGNAL))
		&& !EVBASE_IN_THREAD(thiz)) {
			++thiz->current_event_waiters;
			EVTHREAD_COND_WAIT(thiz->current_event_cond, thiz->th_base_lock);
	}
#endif

	if ((ev->ev_events & (VPK_EV_READ|VPK_EV_WRITE|VPK_EV_SIGNAL|VPK_EV_NOTICE))
		&& !(ev->ev_flags & (VPK_EVLIST_INSERTED|VPK_EVLIST_ACTIVE))) 
	{
		if (ev->ev_events & (VPK_EV_READ|VPK_EV_WRITE))
			ret = evmap_io_add(thiz, ev->ev_fd, ev);
		else if (ev->ev_events & (VPK_EV_SIGNAL))
			;	//...
		else if (ev->ev_events & (VPK_EV_NOTICE))
			evmap_notice_add(thiz, ev->ev_fd, ev);

		if (ret != -1)		//... ret = 0 ?
			event_queue_insert(thiz, ev, VPK_EVLIST_INSERTED);	//...
		if (ret == 1) {
			notify = 1;
			ret = 0;
		}
	}

	if (ret != -1 && tv != NULL) {
		struct timeval now;
		
		/*
		 * for persistent timeout events, we remember the
		 * timeout value and re-add the event.
		 *
		 * If tv_is_absolute, this was already set.
		 */
		//if (ev->ev_closure == EV_CLOSURE_PERSIST && !tv_is_absolute)
		if (ev->ev_closure == VPK_EV_CLOSURE_PERSIST && !tv_is_absolute)
			ev->ev_io_timeout = *tv;

		//...  timeout without callback; active list, need remove and add


		vpk_gettime(thiz, &now);

		if (tv_is_absolute)
			ev->ev_timeout = *tv;
		else
			vpk_timeradd(&now, tv, &ev->ev_timeout);

		EVENT_LOGD(("event_add: timeout in %d(%d/%d) seconds, (now: %d/%d)call %p",
			(int)tv->tv_sec, ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec, now.tv_sec, now.tv_usec, ev->event_callback));

		event_queue_insert(thiz, ev, VPK_EVLIST_TIMEOUT);

		if (minheap_elt_is_top(ev))
			notify = 1;

	}

	if (ret != -1 && notify && EVBASE_NEED_NOTIFY(thiz))
		event_thread_notify(thiz);

	return ret;
}

static int event_del_internal(vpk_events* ev)
{
	vpk_evbase_t* thiz;
	int ret = 0, notify = 0;

	EVENT_LOGD(("event_del: %p (fd %d), callback %p",
		ev, ev->ev_fd, ev->event_callback));

	if (ev->ev_base == NULL)
		return -1;

	thiz = ev->ev_base;

#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
	if (thiz->current_event == ev && !EVBASE_IN_THREAD(thiz)) {
		++thiz->current_event_waiters;
		EVTHREAD_COND_WAIT(thiz->current_event_cond, thiz->th_base_lock);
	}
#endif

	if (ev->ev_flags & VPK_EVLIST_TIMEOUT) {
		event_queue_remove(thiz, ev, VPK_EVLIST_TIMEOUT);
	}

	if (ev->ev_flags & VPK_EVLIST_ACTIVE) {
		event_queue_remove(thiz, ev, VPK_EVLIST_ACTIVE);
	}

	if (ev->ev_flags & VPK_EVLIST_INSERTED) {
		event_queue_remove(thiz, ev, VPK_EVLIST_INSERTED);
		if (ev->ev_events & (VPK_EV_READ|VPK_EV_WRITE))
			ret = evmap_io_del(thiz, ev->ev_fd, ev);
		else if (ev->ev_events & (VPK_EV_SIGNAL))
			;
		else if (ev->ev_events & (VPK_EV_NOTICE))
			evmap_notice_del(thiz, ev->ev_fd, ev);

		if (ret == 1) {
			notify = 1;
			ret = 0;
		}

	}

	if (ret != -1 && notify && EVBASE_NEED_NOTIFY(thiz))
		event_thread_notify(thiz);

	return ret;
}

static int timeout_next(vpk_evbase_t* thiz, struct timeval **tv_p)
{
	struct timeval now;
	vpk_events *ev;
	struct timeval *tv = *tv_p;

	ev = minheap_top(&thiz->timeheap);
	if (ev == NULL) {
		EVENT_LOGI("min heap don't have events");
		*tv_p = NULL;
		return 0;
	}

	if (vpk_gettime(thiz, &now) == -1)
		return -1;

	if (vpk_timercmp(&ev->ev_timeout, &now, <=)) {
		EVENT_LOGW("ev_timeout <= now!");
		vpk_timerclear(tv);
		return 0;
	}

	vpk_timersub(&ev->ev_timeout, &now, tv);
	if (tv->tv_sec < 0) {
		EVENT_LOGW("tv->tv_sec < 0, it's negative!");
		tv->tv_sec = 0;
	}

	EVENT_LOGD(("timeout_next: in %d(%d %d) seconds", 
		(int)tv->tv_sec, ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec));

	return 0;
}

static void timeout_process(vpk_evbase_t *thiz)
{
	struct timeval now;
	vpk_events *ev;

	if (minheap_empty(&thiz->timeheap)) {
		return;
	}

	vpk_gettime(thiz, &now);

	while((ev = minheap_top(&thiz->timeheap))) {

		EVENT_LOGD(("ev_timeout: %d %d, now: %d %d",
			ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec,
			now.tv_sec, now.tv_usec));

		if (vpk_timercmp(&ev->ev_timeout, &now, >))
			break;

		/* delete this event from the queues */
		event_del_internal(ev);
		event_active_nolock(ev, VPK_EV_TIMEOUT, 1);
		//EVENT_LOGD(("timeout_process: call %p", ev->event_callback));
	}
}

static INLINE void event_notice_closure(vpk_evbase_t *thiz, vpk_events *ev)
{
	short ncalls = ev->ev_ncalls;

	EVBASE_RELEASE_LOCK(thiz, th_base_lock);
	while (ncalls) {
		ncalls--;
		ev->ev_ncalls = ncalls;

		(*ev->event_callback)(ev->ev_fd, ev->ev_result, ev->ev_arg);

		//EVBASE_ACQUIRE_LOCK(base, th_base_lock);
		//should_break = base->event_break;
		//EVBASE_RELEASE_LOCK(base, th_base_lock);
	}
}

static void event_persist_closure(vpk_evbase_t *thiz, vpk_events *ev)
{
	void (*timer_event_cb)(int, short, void *);
	int cb_fd;
	short cb_result;
	void *cb_arg;

	if (ev->ev_io_timeout.tv_sec || ev->ev_io_timeout.tv_usec) {
		struct timeval run_at, relative_to, delay, now;

		vpk_gettime(thiz, &now);
		delay = ev->ev_io_timeout;
		if (ev->ev_result & VPK_EV_TIMEOUT) {
			relative_to = ev->ev_timeout;
		} else {
			relative_to = now;
		}
		vpk_timeradd(&relative_to, &delay, &run_at);
		EVENT_LOGD(("ev_timeout: %d %d, delay: %d %d, run_at: %d %d, now: %d %d",
			ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec,
			delay.tv_sec, delay.tv_usec,
			run_at.tv_sec, run_at.tv_usec,
			now.tv_sec, now.tv_usec));
		
		if (vpk_timercmp(&run_at, &now, <)) {
			/* Looks like we missed at least one invocation due to
			 * a clock jump, not running the event loop for a
			 * while, really slow callbacks, or
			 * something. Reschedule relative to now.
			 */
			vpk_timeradd(&now, &delay, &run_at);
		}

		event_add_internal(ev, &run_at, 1);
	}

	timer_event_cb	= ev->event_callback;
	cb_fd			= ev->ev_fd;
	cb_result		= ev->ev_result;
	cb_arg			= ev->ev_arg;

	EVBASE_RELEASE_LOCK(thiz, th_base_lock);

	/** exec the callback **/
	timer_event_cb(cb_fd, cb_result, cb_arg);
}

static int event_process_active_single_queue(vpk_evbase_t *thiz, struct vpk_event_queue *activeq)
{
	int count = 0;
	vpk_events *ev;
	return_val_if_fail(thiz && activeq, -1);

	for (ev = TAILQ_FIRST(activeq); ev; ev = TAILQ_FIRST(activeq)) {
		if (ev->ev_events & VPK_EV_PERSIST)
			event_queue_remove(thiz, ev, VPK_EVLIST_ACTIVE);	//...
		else
			event_del_internal(ev);

		if (!(ev->ev_flags & VPK_EVLIST_INTERNAL))
			++count;

		EVENT_LOGD((
			"event_process_active: event: %p, %s%s%scall %p",
			ev,
			ev->ev_result & VPK_EV_READ ? "VPK_EV_READ " : " ",
			ev->ev_result & VPK_EV_WRITE ? "VPK_EV_WRITE " : " ",
			ev->ev_result & VPK_EV_TIMEOUT ? "VPK_EV_TIMEOUT " : " ",
			ev->event_callback));

#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
		thiz->current_event = ev;
		thiz->current_event_waiters = 0;
#endif

		switch (ev->ev_closure) {
			case VPK_EV_CLOSURE_SIGNAL:
				break;
			case VPK_EV_CLOSURE_NOTICE:
				event_notice_closure(thiz, ev);
				break;
			case VPK_EV_CLOSURE_PERSIST:
				event_persist_closure(thiz, ev);
				break;
			case VPK_EV_CLOSURE_NONE:
			default:
				EVBASE_RELEASE_LOCK(thiz, th_base_lock);
				(*ev->event_callback)(ev->ev_fd, ev->ev_result, ev->ev_arg);
				break;
		}
		EVBASE_ACQUIRE_LOCK(thiz, th_base_lock);
#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
		thiz->current_event = NULL;
		if (thiz->current_event_waiters) {
			thiz->current_event_waiters = 0;
			EVTHREAD_COND_BROADCAST(thiz->current_event_cond);
		}
#endif
	}

	return count;
}

static int event_process_active(vpk_evbase_t *thiz)
{
	int i = 0;
	struct vpk_event_queue *activeq = NULL;

	for (i = 0; i < thiz->nactivequeues; i++) {
		if (TAILQ_FIRST(&thiz->activequeues[i]) != NULL) {
			activeq = &thiz->activequeues[i];
			event_process_active_single_queue(thiz, activeq);
		}
	}

	return 0;
}

int vpk_event_assign(vpk_events *ev, vpk_evbase_t *base, int fd, short events, vpk_event_callback callback, void *arg)
{
	if (!base) {
		EVENT_LOGE("base is null!");
		return -1;
	}

	ev->ev_base = base;

	ev->event_callback	= callback;
	ev->ev_arg			= arg;
	ev->ev_fd			= fd;
	ev->ev_events		= events;
	ev->ev_result		= 0;
	ev->ev_flags		= VPK_EVLIST_INIT;
	ev->ev_ncalls		= 0;

	if (events & VPK_EV_SIGNAL) {
		if (events & (VPK_EV_READ | VPK_EV_WRITE)) {
			EVENT_LOGW("VPK_EV_SIGNAL is not compatible with read/write");
			return -1;
		}
		ev->ev_closure = VPK_EV_CLOSURE_SIGNAL;
	} else if (events & VPK_EV_NOTICE) {
		if (events & (VPK_EV_READ | VPK_EV_WRITE)) {
			EVENT_LOGW("VPK_EV_NOTICE is not compatible with read/write");
			return -1;
		}
		ev->ev_closure = VPK_EV_CLOSURE_NOTICE;
	} else {
		if (events & VPK_EV_PERSIST) {
			vpk_timerclear(&ev->ev_io_timeout);
			ev->ev_closure = VPK_EV_CLOSURE_PERSIST;
		} else {
			ev->ev_closure = VPK_EV_CLOSURE_NONE;
		}
	}

	minheap_elem_init(ev);

	ev->ev_priority = base->nactivequeues / 2;

	return 0;
}

vpk_events *vpk_event_new(vpk_evbase_t *base, int fd, short events, vpk_event_callback callback, void *arg)
{
	vpk_events *ev;
	ev = VPK_MALLOC(sizeof(vpk_events));
	if (ev) {
		if (vpk_event_assign(ev, base, fd, events, callback, arg) < 0) {
			VPK_FREE(ev);
			return NULL;
		}
	}
	return ev;
}

void vpk_event_free(vpk_events *ev)
{
	if (ev)
	{
		vpk_event_del(ev);
		VPK_FREE(ev);
	}
}

int vpk_event_add(vpk_events *ev, const struct timeval *tv)
{
	int ret = 0;
	if (!ev->ev_base) {
		EVENT_LOGE("event has no evbase set.");
		return -1;
	}

	EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	ret = event_add_internal(ev, tv, 0);

	EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

	return ret;
}

int vpk_event_del(vpk_events *ev)
{
	int ret = 0;
	if (!ev->ev_base) {
		EVENT_LOGE("event has no evbase set.");
		return -1;
	}

	EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	ret = event_del_internal(ev);

	EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

	return ret;
}

//#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
//int event_global_setup_locks(const int enable_locks)
//{
//	if (evmsg_global_setup_locks(enable_locks) < 0)
//		return -1;
//	return 0;
//}
//#endif

vpk_evbase_t* vpk_evbase_create(void)
{
	vpk_evbase_t *base;

	if ((base = (vpk_evbase_t*)VPK_CALLOC(1, sizeof(vpk_evbase_t))) == NULL) {
		EVENT_LOGE("calloc error!");
		return NULL;
	}
	event_thread_use_pthreads();

	detect_monotonic();
	vpk_gettime(base, &base->timer_tv);
	
	minheap_ctor(&base->timeheap);
	TAILQ_INIT(&base->eventqueue);
	base->th_notify_fd[0] = -1;
	base->th_notify_fd[1] = -1;

	evmap_iomap_init(&base->iomap);
	evmap_noticemap_init(&base->noticemap);

	base->evsel = eventops[0];
	base->priv = base->evsel->init(base);
	if (base->priv == NULL) {
		EVENT_LOGW("no event io mechanism available.");
		base->evsel = NULL;
		vpk_evbase_destroy(base);
		return NULL;
	}

	if (event_priority_init(base, 1) < 0) {
		vpk_evbase_destroy(base);
		return NULL;
	}

	// thread
#ifndef VPK_EVENT_DISABLE_THREAD_SUPPORT
	int r = 0;
	EVTHREAD_ALLOC_LOCK(base->th_base_lock, VPK_THREAD_LOCKTYPE_RECURSIVE);
	EVTHREAD_ALLOC_COND(base->current_event_cond);
	r = event_thread_make_notifiable(base);
	if (r < 0) {
		EVENT_LOGW("unable to make notifiable.");
		vpk_evbase_destroy(base);
		return NULL;
	}

	//event_global_setup_locks(1);
#endif

	return base;
}

int vpk_evbase_loop(vpk_evbase_t* thiz, int flags)
{
	struct timeval tv = {0};
	struct timeval *tv_p;
	int res, done, ret = 0;

	EVBASE_ACQUIRE_LOCK(thiz, th_base_lock);
	if (thiz->running_loop) {
		EVENT_LOGW("already running loop, only one can run on evbase at once.");
		EVBASE_RELEASE_LOCK(thiz, th_base_lock);
		return -1;
	}

	thiz->running_loop = 1;
	done = 0;

	thiz->th_owner_id = EVTHREAD_GET_ID();

	while (!done) {
		tv_p = &tv;

		EVENT_LOGD(("=========111event_count_active %d event cnt %d", thiz->event_count_active, thiz->event_count));
		if (!thiz->event_count_active) {
			timeout_next(thiz, &tv_p);
		} else {
			vpk_timerclear(&tv);
		}
		EVENT_LOGD(("=========222event_count_active %d event cnt %d, tv: %d/%d", 
			thiz->event_count_active, thiz->event_count, tv.tv_sec, tv.tv_usec));

		if (!thiz->event_count_active && !(thiz->event_count > 0)) {
			EVENT_LOGI("no events registered.");
			sleep(5);
		}

		vpk_gettime(thiz, &thiz->timer_tv);
#if 0
		sleep((int)(tv.tv_sec));	// dispatch epoll ...
#else
		//struct timeval temp;
		//temp.tv_sec = tv.tv_sec;
		//temp.tv_usec = tv.tv_usec;
		//select(0, NULL, NULL, NULL, &tv);
		
		res = thiz->evsel->dispatch(thiz, tv_p);
		if (res == -1) {
			EVENT_LOGD(("dispatch returned failed."));
			ret = 0;
			//goto done;
		}
#endif

		timeout_process(thiz);

		//EVENT_LOGD(("=========333event_count_active %d event cnt %d", thiz->event_count_active, thiz->event_count));
		if (thiz->event_count_active) {
			event_process_active(thiz);
		}
		//EVENT_LOGD(("=========444event_count_active %d, %d seconds", thiz->event_count_active, (int)tv.tv_sec));
	}

	EVBASE_RELEASE_LOCK(thiz, th_base_lock);

	return ret;
}

void vpk_evbase_destroy(vpk_evbase_t* thiz)
{
	int i, n_deleted = 0;
	vpk_events* ev;
	if (thiz == NULL) {
		EVENT_LOGW("no evbase to free");
		return;
	}

	if (thiz->th_notify_fd[0] != -1) {
		vpk_event_del(&thiz->th_notify);
		close(thiz->th_notify_fd[0]);
		if (thiz->th_notify_fd[1] != -1)
			close(thiz->th_notify_fd[1]);
		thiz->th_notify_fd[0] = -1;
		thiz->th_notify_fd[1] = -1;
	}

	while((ev = minheap_top(&thiz->timeheap))) {
		vpk_event_del(ev);
		n_deleted++;
	}
	minheap_dtor(&thiz->timeheap);

	for (i = 0; i < thiz->nactivequeues; ++i) {
		for (ev = TAILQ_FIRST(&thiz->activequeues[i]); ev; ) {
			vpk_events *next = TAILQ_NEXT(ev, ev_active_next);
			if (!(ev->ev_flags & VPK_EVLIST_INTERNAL)) {
				vpk_event_del(ev);
				++n_deleted;
			}
			ev = next;
		}
	}

	if (n_deleted) {
		EVENT_LOGD(("%d events were still set in evbase."));
	}

	if (thiz->evsel && thiz->evsel->dealloc)
		thiz->evsel->dealloc(thiz);

	evmap_iomap_clear(&thiz->iomap);
	evmap_noticemap_clear(&thiz->noticemap);

	EVTHREAD_FREE_LOCK(thiz->th_base_lock, VPK_THREAD_LOCKTYPE_RECURSIVE);
	EVTHREAD_FREE_COND(thiz->current_event_cond);

	VPK_FREE(thiz->activequeues);
	VPK_FREE(thiz);
}

static int event_thread_notify(vpk_evbase_t *thiz)
{
	if (!thiz->th_notify_func) {
		EVENT_LOGE("th_notify_func is null.");
		return -1;
	}
	if (thiz->is_notify_pending)
		return 0;
	thiz->is_notify_pending = 1;
	return thiz->th_notify_func(thiz);
}

static int event_thread_notify_default(vpk_evbase_t *thiz)
{
	uint64_t msg = 1;
	int r;
	do {
		r = write(thiz->th_notify_fd[0], (void*) &msg, sizeof(msg));
	} while (r < 0 && errno == EAGAIN);

	EVENT_LOGD(("notify write msg, ret = %d", r));
	return (r < 0) ? -1 : 0;
}

static void event_thread_drain_default(int fd, short what, void *arg)
{
	uint64_t msg;
	ssize_t r;
	vpk_evbase_t *thiz = arg;

	r = read(fd, (void*) &msg, sizeof(msg));
	if ( r < 0 && errno != EAGAIN) {
		EVENT_LOGW("Error reading from eventfd");
	}
	EVENT_LOGD(("read: %d", msg));
	EVBASE_ACQUIRE_LOCK(thiz, th_base_lock);
	thiz->is_notify_pending = 0;
	EVBASE_RELEASE_LOCK(thiz, th_base_lock);
}

int event_thread_make_notifiable(vpk_evbase_t *thiz)
{
	void (*rcallback)(int, short, void *) = event_thread_drain_default;
	int (*notify)(vpk_evbase_t *) = event_thread_notify_default;

	if (!thiz) {
		EVENT_LOGE("null pointer error.");
		return -1;
	}

	if (thiz->th_notify_fd[0] >= 0)
		return 0;

	thiz->th_notify_fd[0] = eventfd(0, EFD_CLOEXEC);
	if (thiz->th_notify_fd[0] >= 0) {
		vpk_socket_closeonexec(thiz->th_notify_fd[0]);
		rcallback = event_thread_drain_default;
		notify = event_thread_notify_default;
	}

	if (thiz->th_notify_fd[0] < 0) {
		EVENT_LOGW("fd errrrrr..");
		//... use socketpair
	}

	vpk_socket_nonblocking(thiz->th_notify_fd[0]);
	if (thiz->th_notify_fd[1] > 0)
		vpk_socket_nonblocking(thiz->th_notify_fd[1]);

	thiz->th_notify_func = notify;

	vpk_event_assign(&thiz->th_notify, thiz, thiz->th_notify_fd[0],
						VPK_EV_READ|VPK_EV_PERSIST, rcallback, thiz);

	thiz->th_notify.ev_flags |= VPK_EVLIST_INTERNAL;
	thiz->th_notify.ev_priority = 0;	

	return vpk_event_add(&thiz->th_notify, NULL);
}
