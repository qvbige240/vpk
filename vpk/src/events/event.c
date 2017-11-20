/**
 * History:
 * ================================================================
 * 2017-08-03 qing.zou created
 *
 */

#include <time.h>
#include <sys/time.h>

#include "vpk_events.h"
#include "event.h"

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
	TIMER_LOGD(("mono ts: %d %ld", ts.tv_sec, ts.tv_nsec));

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
			//TIMER_LOGD(("real time: %d %d, clock mono: %d %ld, diff: %d %d",
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
		TIMER_LOGE("event(%p) already on queue %x", ev, queue);
		return;
	}

	if (~ev->ev_flags & VPK_EVLIST_INTERNAL)
		base->event_count++;

	ev->ev_flags |= queue;
	switch (queue) {
		case VPK_EVLIST_INSERTED:
			break;
		case VPK_EVLIST_ACTIVE:
			base->event_count_active++;
			TAILQ_INSERT_TAIL(&base->activequeues[ev->ev_priority], ev, ev_active_next);
			break;
		case VPK_EVLIST_TIMEOUT:
			minheap_push(&base->timeheap, ev);
			break;
		default:
			TIMER_LOGE("unknown queue %x", queue);
			break;
	}
}

static void event_queue_remove(vpk_evbase_t* base, vpk_events* ev, int queue)
{
	if (!(ev->ev_flags & queue)) {
		TIMER_LOGE("event(%p) already on queue %x", ev, queue);
		return;
	}

	if (~ev->ev_flags & VPK_EVLIST_INTERNAL)
		base->event_count--;

	ev->ev_flags &= ~queue;
	switch (queue) {
		case VPK_EVLIST_INSERTED:
			break;
		case VPK_EVLIST_ACTIVE:
			base->event_count_active--;
			TAILQ_REMOVE(&base->activequeues[ev->ev_priority], ev, ev_active_next);
			break;
		case VPK_EVLIST_TIMEOUT:
			minheap_erase(&base->timeheap, ev);
			break;
		default:
			TIMER_LOGE("unknown queue %x", queue);
			break;
	}
}

static void event_active_nolock(vpk_events *ev, int flag)
{
	vpk_evbase_t *thiz;

	TIMER_LOGD(("event_active: %p (fd %d), flag 0x%02x, callback %p", ev, (ev->ev_fd), (int)flag, ev->event_callback));

	if (ev->ev_flags & VPK_EVLIST_ACTIVE) {
		TIMER_LOGW("event already be in active!");
		return;
	}

	thiz = ev->ev_base;
	if (thiz == NULL) {
		TIMER_LOGW("ev->ev_base is NULL!");
		return;
	}
	ev->ev_result = flag;

	event_queue_insert(thiz, ev, VPK_EVLIST_ACTIVE);
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
		TIMER_LOGE("calloc failed!");
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
	int notify = 0;
	vpk_evbase_t* thiz = ev->ev_base;

	TIMER_LOGD(("event_add: event: %p (fd %d), %s%s%s call %p",
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

	if (tv != NULL) {
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

		vpk_gettime(thiz, &now);

		if (tv_is_absolute)
			ev->ev_timeout = *tv;
		else
			vpk_timeradd(&now, tv, &ev->ev_timeout);

		TIMER_LOGD(("event_add: timeout in %d(%d) seconds, (now: %d %d)call %p",
			(int)tv->tv_sec, ev->ev_timeout.tv_sec, now.tv_sec, now.tv_usec, ev->event_callback));

		event_queue_insert(thiz, ev, VPK_EVLIST_TIMEOUT);

		if (minheap_elt_is_top(ev))
			notify = 1;

		// notify thread...
	}

	return 0;
}

static int event_del_internal(vpk_events* ev)
{
	vpk_evbase_t* thiz;

	TIMER_LOGD(("event_del: %p (fd %d), callback %p",
		ev, ev->ev_fd, ev->event_callback));

	if (ev->ev_base == NULL)
		return -1;

	thiz = ev->ev_base;

	if (ev->ev_flags & VPK_EVLIST_TIMEOUT) {
		event_queue_remove(thiz, ev, VPK_EVLIST_TIMEOUT);
	}

	if (ev->ev_flags & VPK_EVLIST_ACTIVE) {
		event_queue_remove(thiz, ev, VPK_EVLIST_ACTIVE);
	}

	return 0;
}

static int timeout_next(vpk_evbase_t* thiz, struct timeval **tv_p)
{
	struct timeval now;
	vpk_events *ev;
	struct timeval *tv = *tv_p;

	ev = minheap_top(&thiz->timeheap);
	if (ev == NULL) {
		TIMER_LOGI("min heap don't have events");
		*tv_p = NULL;
		return 0;
	}

	if (vpk_gettime(thiz, &now) == -1)
		return -1;

	if (vpk_timercmp(&ev->ev_timeout, &now, <=)) {
		TIMER_LOGW("ev_timeout <= now!");
		vpk_timerclear(tv);
		return 0;
	}

	vpk_timersub(&ev->ev_timeout, &now, tv);
	if (tv->tv_sec < 0) {
		TIMER_LOGW("tv->tv_sec < 0, it's negative!");
		tv->tv_sec = 0;
	}

	TIMER_LOGD(("timeout_next: in %d(%d %d) seconds", 
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

		TIMER_LOGD(("ev_timeout: %d %d, now: %d %d",
			ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec,
			now.tv_sec, now.tv_usec));

		if (vpk_timercmp(&ev->ev_timeout, &now, >))
			break;

		/* delete this event from the queues */
		event_del_internal(ev);
		event_active_nolock(ev, VPK_EV_TIMEOUT);
		//TIMER_LOGD(("timeout_process: call %p", ev->event_callback));
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
		TIMER_LOGD(("ev_timeout: %d %d, delay: %d %d, run_at: %d %d, now: %d %d",
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

	// release lock
	// ...

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

		TIMER_LOGD((
			"event_process_active: event: %p, %s%s%scall %p",
			ev,
			ev->ev_result & VPK_EV_READ ? "VPK_EV_READ " : " ",
			ev->ev_result & VPK_EV_WRITE ? "VPK_EV_WRITE " : " ",
			ev->ev_result & VPK_EV_TIMEOUT ? "VPK_EV_TIMEOUT " : " ",
			ev->event_callback));

		switch (ev->ev_closure) {
			case VPK_EV_CLOSURE_SIGNAL:
				break;
			case VPK_EV_CLOSURE_PERSIST:
				event_persist_closure(thiz, ev);
				break;
			case VPK_EV_CLOSURE_NONE:
				//break;
			default:
				// release lock ...
				(*ev->event_callback)(ev->ev_fd, ev->ev_result, ev->ev_arg);
				break;
		}
		// lock ...
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
		TIMER_LOGE("base is null!");
		return -1;
	}

	ev->ev_base = base;

	ev->event_callback	= callback;
	ev->ev_arg			= arg;
	ev->ev_fd			= fd;
	ev->ev_events		= events;
	ev->ev_result		= 0;
	ev->ev_flags		= VPK_EVLIST_INIT;

	if (events & VPK_EV_PERSIST) {
		vpk_timerclear(&ev->ev_io_timeout);
		ev->ev_closure = VPK_EV_CLOSURE_PERSIST;
	} else {
		ev->ev_closure = VPK_EV_CLOSURE_NONE;
	}

	minheap_elem_init(ev);

	ev->ev_priority = base->nactivequeues / 2;

	return 0;
}

int vpk_event_add(vpk_events *ev, const struct timeval *tv)
{
	int ret = 0;
	if (!ev->ev_base) {
		TIMER_LOGE("event has no evbase set.");
		return -1;
	}

	// lock ...
	ret = event_add_internal(ev, tv, 0);
	// release lock ...

	return ret;
}

int vpk_event_del(vpk_events *ev)
{
	int ret = 0;
	if (!ev->ev_base) {
		TIMER_LOGE("event has no evbase set.");
		return -1;
	}

	// lock ...
	ret = event_del_internal(ev);
	// release lock ...

	return ret;
}

vpk_evbase_t* vpk_evbase_create(void)
{
	vpk_evbase_t* base;

	if ((base = (vpk_evbase_t*)VPK_CALLOC(1, sizeof(vpk_evbase_t))) == NULL) {
		TIMER_LOGE("calloc error!");
		return NULL;
	}

	detect_monotonic();
	vpk_gettime(base, &base->timer_tv);
	
	minheap_ctor(&base->timeheap);
	if (event_priority_init(base, 1) < 0) {
		vpk_evbase_destroy(base);
		return NULL;
	}

	return base;
}

int vpk_evbase_loop(vpk_evbase_t* thiz, int flags)
{
	struct timeval tv;
	struct timeval *tv_p;
	int done;

	// lock ...
	if (thiz->running_loop) {
		TIMER_LOGW("already running loop, only one can run on evbase at once.");
		// release lock ...
		return -1;
	}

	thiz->running_loop = 1;
	done = 0;

	while (!done) {
		tv_p = &tv;

		//TIMER_LOGD(("=========111event_count_active %d event cnt %d", thiz->event_count_active, thiz->event_count));
		if (!thiz->event_count_active) {
			timeout_next(thiz, &tv_p);
		} else {
			vpk_timerclear(&tv);
		}
		//TIMER_LOGD(("=========222event_count_active %d event cnt %d", thiz->event_count_active, thiz->event_count));

		if (!thiz->event_count_active && !(thiz->event_count > 0)) {
			TIMER_LOGI("no events registered.");
			sleep(5);
		}

		vpk_gettime(thiz, &thiz->timer_tv);
#if 0
		sleep((int)(tv.tv_sec));	// dispatch epoll ...
#else
		//struct timeval temp;
		//temp.tv_sec = tv.tv_sec;
		//temp.tv_usec = tv.tv_usec;
		select(0, NULL, NULL, NULL, &tv);
#endif

		timeout_process(thiz);

		//TIMER_LOGD(("=========333event_count_active %d event cnt %d", thiz->event_count_active, thiz->event_count));
		if (thiz->event_count_active) {
			event_process_active(thiz);
		}
		//TIMER_LOGD(("=========444event_count_active %d, %d seconds", thiz->event_count_active, (int)tv.tv_sec));
	}

	return 0;
}

void vpk_evbase_destroy(vpk_evbase_t* thiz)
{
	int i, n_deleted = 0;
	vpk_events* ev;
	if (thiz == NULL) {
		TIMER_LOGW("no evbase to free");
		return;
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
		TIMER_LOGD(("%d events were still set in evbase."));
	}

	VPK_FREE(thiz->activequeues);
	VPK_FREE(thiz);
}
