/**
 * History:
 * ================================================================
 * 2017-08-03 qing.zou created
 *
 */

#include <time.h>
#include <sys/time.h>

#include "vpk_timer.h"
#include "timer_event.h"

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

static int vpk_gettime(vpk_timer_t* timer, struct timeval *tp)
{
#if defined(VPK_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	if (use_monotonic) {
		struct timespec	ts;

		if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
			return (-1);

		tp->tv_sec = ts.tv_sec;
		tp->tv_usec = ts.tv_nsec / 1000;
		if (timer->last_updated_clock_diff + CLOCK_SYNC_INTERVAL < ts.tv_sec) {
			struct timeval tv;
			vpk_gettimeofday(&tv,NULL);
			vpk_timersub(&tv, tp, &timer->tv_clock_diff);
			timer->last_updated_clock_diff = ts.tv_sec;
			TIMER_LOGD(("real time: %d %d, clock mono: %d %ld, diff: %d %d",
				tv.tv_sec, tv.tv_usec, 
				ts.tv_sec, ts.tv_nsec, 
				timer->tv_clock_diff.tv_sec, timer->tv_clock_diff.tv_usec));
		}

		return (0);
	}
#endif

	return (vpk_gettimeofday(tp, NULL));
}

static void event_queue_insert(vpk_timer_t* timer, vpk_events* ev, int queue)
{
	if ((ev->ev_flags & queue)) {
		if (queue & VPK_EVLIST_ACTIVE)
			return;
		TIMER_LOGE("event(%p) already on queue %x", ev, queue);
		return;
	}

	if (~ev->ev_flags & VPK_EVLIST_INTERNAL)
		timer->event_count++;

	ev->ev_flags |= queue;
	switch (queue) {
		case VPK_EVLIST_INSERTED:
			break;
		case VPK_EVLIST_ACTIVE:
			timer->event_count_active++;
			TAILQ_INSERT_TAIL(&timer->activequeues[ev->ev_priority], ev, ev_active_next);
			break;
		case VPK_EVLIST_TIMEOUT:
			minheap_push(&timer->timeheap, ev);
			break;
		default:
			TIMER_LOGE("unknown queue %x", queue);
			break;
	}
}

static void event_queue_remove(vpk_timer_t* timer, vpk_events* ev, int queue)
{
	if (!(ev->ev_flags & queue)) {
		TIMER_LOGE("event(%p) already on queue %x", ev, queue);
		return;
	}

	if (~ev->ev_flags & VPK_EVLIST_INTERNAL)
		timer->event_count--;

	ev->ev_flags &= ~queue;
	switch (queue) {
		case VPK_EVLIST_INSERTED:
			break;
		case VPK_EVLIST_ACTIVE:
			timer->event_count_active--;
			TAILQ_REMOVE(&timer->activequeues[ev->ev_priority], ev, ev_active_next);
			break;
		case VPK_EVLIST_TIMEOUT:
			minheap_erase(&timer->timeheap, ev);
			break;
		default:
			TIMER_LOGE("unknown queue %x", queue);
			break;
	}
}

static void event_active_nolock(vpk_events *ev, int flag)
{
	vpk_timer_t *thiz;

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

static int vpk_timer_priority_init(vpk_timer_t* timer, int npriorities)
{
	int i = 0;
	if (timer->event_count_active || npriorities < 1 || npriorities >= 256)
		return -1;

	if (npriorities == timer->nactivequeues)
		return 0;

	if (timer->nactivequeues) {
		VPK_FREE(timer->activequeues);
		timer->nactivequeues = 0;
	}

	timer->activequeues = (struct vpk_event_queue *)VPK_CALLOC(npriorities, sizeof(struct vpk_event_queue));
	if (timer->activequeues == NULL) {
		TIMER_LOGE("calloc failed!");
		return -1;
	}
	timer->nactivequeues = npriorities;

	for (i = 0; i < timer->nactivequeues; i++)	{
		TAILQ_INIT(&timer->activequeues[i]);
	}

	return 0;
}

static int event_add_internal(vpk_events* ev, const struct timeval* tv, int tv_is_absolute)
{
	int notify = 0;
	vpk_timer_t* thiz = ev->ev_base;

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

		TIMER_LOGD(("event_add: timeout in %d(%d) seconds, call %p",
			(int)tv->tv_sec, ev->ev_timeout.tv_sec, ev->event_callback));

		event_queue_insert(thiz, ev, VPK_EVLIST_TIMEOUT);

		if (minheap_elt_is_top(ev))
			notify = 1;

		// notify thread...
	}

	return 0;
}

static int event_del_internal(vpk_events* ev)
{
	vpk_timer_t* thiz;

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

static int timeout_next(vpk_timer_t* thiz, struct timeval **tv_p)
{
	struct timeval now;
	vpk_events *ev;
	struct timeval *tv = *tv_p;

	ev = minheap_top(&thiz->timeheap);
	if (ev == NULL) {
		*tv_p = NULL;
		return 0;
	}

	if (vpk_gettime(thiz, &now) == -1)
		return -1;

	if (vpk_timercmp(&ev->ev_timeout, &now, <=)) {
		TIMER_LOGW("ev_timeout <= tv!");
		vpk_timerclear(tv);
		return 0;
	}

	vpk_timersub(&ev->ev_timeout, &now, tv);
	if (tv->tv_sec < 0) {
		TIMER_LOGW("tv->tv_sec < 0, it's negative!");
		tv->tv_sec = 0;
	}

	TIMER_LOGD(("timeout_next: in %d seconds", (int)tv->tv_sec));

	return 0;
}

static void timeout_process(vpk_timer_t *thiz)
{
	struct timeval now;
	vpk_events *ev;

	if (minheap_empty(&thiz->timeheap)) {
		return;
	}

	vpk_gettime(thiz, &now);

	while((ev = minheap_top(&thiz->timeheap))) {

		TIMER_LOGD(("ev_timeout: %d %d, now: %d %d\n",
			ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec,
			now.tv_sec, now.tv_usec));

		if (vpk_timercmp(&ev->ev_timeout, &now, >))
			break;

		/* delete this event from the queues */
		event_del_internal(ev);
		event_active_nolock(ev, VPK_EV_TIMEOUT);
		TIMER_LOGD(("timeout_process: call %p", ev->event_callback));
	}
}

static void event_persist_closure(vpk_timer_t *thiz, vpk_events *ev)
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
		TIMER_LOGD(("ev_timeout: %d %d, delay: %d %d, run_at: %d %d\n",
			ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec,
			delay.tv_sec, delay.tv_usec,
			run_at.tv_sec, run_at.tv_usec));
		
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

static int event_process_active_single_queue(vpk_timer_t *thiz, struct vpk_event_queue *activeq)
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

static int event_process_active(vpk_timer_t *thiz)
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

int vpk_event_assign(vpk_events *ev, vpk_timer_t *base, int fd, short events, vpk_event_callback callback, void *arg)
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

int vpk_timer_event_add(vpk_events *ev, const struct timeval *tv)
{
	int ret = 0;
	if (!ev->ev_base) {
		TIMER_LOGE("event has no base timer set.");
		return -1;
	}

	// lock ...
	ret = event_add_internal(ev, tv, 0);
	// release lock ...

	return ret;
}

vpk_timer_t* vpk_timer_create(void)
{
	vpk_timer_t* timer;

	if ((timer = (vpk_timer_t*)VPK_CALLOC(1, sizeof(vpk_timer_t))) == NULL) {
		TIMER_LOGE("calloc error!");
		return NULL;
	}

	detect_monotonic();
	vpk_gettime(timer, &timer->timer_tv);
	
	minheap_ctor(&timer->timeheap);
	if (vpk_timer_priority_init(timer, 1) < 0) {
		vpk_timer_destroy(timer);
		return NULL;
	}

	return timer;
}

int vpk_timer_loop(vpk_timer_t* thiz, int flags)
{
	struct timeval tv;
	struct timeval *tv_p;
	int done;

	// lock ...
	if (thiz->running_loop) {
		TIMER_LOGW("already running loop, only one can run on timer at once.");
		// release lock ...
		return -1;
	}

	thiz->running_loop = 1;
	done = 0;

	while (!done) {
		tv_p = &tv;

		TIMER_LOGD(("=========111event_count_active %d", thiz->event_count_active));
		if (!thiz->event_count_active) {
			timeout_next(thiz, &tv_p);
		} else {
			vpk_timerclear(&tv);
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

		TIMER_LOGD(("=========22event_count_active %d, %d seconds", thiz->event_count_active, (int)tv.tv_sec));
		if (thiz->event_count_active) {
			event_process_active(thiz);
		}
		TIMER_LOGD(("=========333event_count_active %d, %d seconds", thiz->event_count_active, (int)tv.tv_sec));
	}

	return 0;
}

void vpk_timer_destroy(vpk_timer_t* thiz)
{
	vpk_events* ev;
	if (thiz == NULL)
		return;

}
