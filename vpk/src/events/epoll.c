/**
 * History:
 * ================================================================
 * 2017-11-28 qing.zou created
 *
 */

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <sys/epoll.h>

#include "event.h"
#include "event_msg.h"
#include "event_map.h"
#include "event_thread.h"

struct priv_info {
	struct epoll_event *events;
	int nevents;
	int epfd;
};

#define EVENT_INITIAL_NEVENT		32
#define EVENT_MAX_NEVENT			4096

/* On Linux kernels at least up to 2.6.24.4, epoll can't handle timeout
 * values bigger than (LONG_MAX - 999ULL)/HZ.  HZ in the wild can be
 * as big as 1000, and LONG_MAX can be as small as (1<<31)-1, so the
 * largest number of msec we can support here is 2147482.  Let's
 * round that down by 47 seconds.
 */
#define MAX_EPOLL_TIMEOUT_MSEC		(35*60*1000)
#define MAX_SECONDS_IN_MSEC_LONG	(((LONG_MAX) - 999) / 1000)

static long event_tv_to_msec(const struct timeval *tv)
{
	if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LONG)
		return -1;

	return (tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000);
}

static void *epoll_init(vpk_evbase_t *thiz)
{
	int epfd;
	struct priv_info *priv;

	if ((epfd = epoll_create(32000)) == -1) {
		if (errno != ENOSYS)
			EVENT_LOGW("epoll_create");
		return NULL;
	}

	vpk_socket_closeonexec(epfd);

	if (!(priv = VPK_CALLOC(1, sizeof(struct priv_info)))) {
		close(epfd);
	}

	priv->epfd = epfd;
	priv->events = VPK_CALLOC(EVENT_INITIAL_NEVENT, sizeof(struct epoll_event));
	if (priv->events == NULL) {
		VPK_FREE(priv);
		close(epfd);
		return NULL;
	}
	priv->nevents = EVENT_INITIAL_NEVENT;

	evmsg_init(thiz);

	return priv;
}

static const char *change_to_string(int change)
{
	change &= (VPK_EVCHANGE_ADD|VPK_EVCHANGE_DEL);
	if (change == VPK_EVCHANGE_ADD) {
		return "add";
	} else if (change == VPK_EVCHANGE_DEL) {
		return "del";
	} else if (change == 0) {
		return "none";
	} else {
		return "???";
	}
}

static const char *epoll_op_to_string(int op)
{
	return op == EPOLL_CTL_ADD ? "ADD":
		op == EPOLL_CTL_DEL ? "DEL":
		op == EPOLL_CTL_MOD ? "MOD":
		"???";
}

static int epoll_apply_one_change(vpk_evbase_t *thiz, struct priv_info *priv, struct event_change *change)
{
	struct epoll_event epev;
	int op, events = 0;

	if (1)
	{
		if ((change->read_change & VPK_EVCHANGE_ADD) ||
			(change->write_change & VPK_EVCHANGE_ADD))
		{
			events = 0;
			op = EPOLL_CTL_ADD;
			if (change->read_change & VPK_EVCHANGE_ADD) {
				events |= EPOLLIN;
			} else if (change->read_change & VPK_EVCHANGE_DEL) {
				;
			} else if (change->old_events & VPK_EV_READ) {
				events |= EPOLLIN;
			}
			if (change->write_change & VPK_EVCHANGE_ADD) {
				events |= EPOLLOUT;
			} else if (change->write_change & VPK_EVCHANGE_DEL) {
				;
			} else if (change->old_events & VPK_EV_WRITE) {
				events |= EPOLLOUT;
			}
			if ((change->read_change|change->write_change) & VPK_EV_ET)
				events |= EPOLLET;

			if (change->old_events) {
				op = EPOLL_CTL_MOD;
			}
		} else if ((change->read_change & VPK_EVCHANGE_DEL) ||
			(change->write_change & VPK_EVCHANGE_DEL))
		{
			op = EPOLL_CTL_DEL;
			
			if (change->read_change & VPK_EVCHANGE_DEL) {
				if (change->write_change & VPK_EVCHANGE_DEL) {
					events = EPOLLIN|EPOLLOUT;
				} else if (change->old_events & VPK_EV_WRITE) {
					events = EPOLLOUT;
					op = EPOLL_CTL_MOD;
				} else {
					events = EPOLLIN;
				}
			} else if (change->write_change & VPK_EVCHANGE_DEL) {
				if (change->old_events & VPK_EV_READ) {
					events = EPOLLIN;
					op = EPOLL_CTL_MOD;
				} else {
					events = EPOLLOUT;
				}
			}
		}

		if (!events)
			return 0;

		memset(&epev, 0, sizeof(epev));
		epev.data.fd = change->fd;
		epev.events = events;
		if (epoll_ctl(priv->epfd, op, change->fd, &epev) == -1) {
			if (op == EPOLL_CTL_MOD && errno == ENOENT) {
				/* the fd was probably closed and re-opened */
				if (epoll_ctl(priv->epfd, EPOLL_CTL_ADD, change->fd, &epev) == -1) {
					EVENT_LOGW("Epoll MOD(%d) on %d retried as ADD, failed too.", (int)epev.events, change->fd);
					return -1;
				} else {
					EVENT_LOGI("Epoll MOD(%d) on %d retried as ADD, successed.", (int)epev.events, change->fd);
				}
			} else if (op == EPOLL_CTL_ADD && errno == EEXIST) {
				/* maybe using dup*() to duplicate the same file into the same fd */
				if (epoll_ctl(priv->epfd, EPOLL_CTL_MOD, change->fd, &epev) == -1) {
					EVENT_LOGW("Epoll ADD(%d) on %d retried as MOD, failed too.", (int)epev.events, change->fd);
					return -1;
				} else {
					EVENT_LOGI("Epoll ADD(%d) on %d retried as MOD, successed.", (int)epev.events, change->fd);
				}
			} else if (op == EPOLL_CTL_DEL && (errno == ENOENT || errno == EBADF || errno == EPERM)) {
				EVENT_LOGI("Epoll DEL(%d) on fd %d gave %s: DEL was unnecessary.",
					(int)epev.events,
					change->fd,
					strerror(errno));
			} else {
				EVENT_LOGW("Epoll %s(%d) on fd %d failed. Old events were %d; read change was %d (%s); write change was %d (%s)",
					epoll_op_to_string(op),
					(int)epev.events,
					change->fd,
					change->old_events,
					change->read_change,
					change_to_string(change->read_change),
					change->write_change,
					change_to_string(change->write_change));
				return -1;
			}
		} else {
			EVENT_LOGD(("Epoll %s(%d) on fd %d okay. Old events were %d; read change was %d; write change was %d",
				epoll_op_to_string(op),
				(int)epev.events,
				(int)change->fd,
				change->old_events,
				change->read_change,
				change->write_change));
		}
	}

	return 0;
}

static int epoll_change_add(vpk_evbase_t *thiz, int fd, short old, short events, void *p)
{
	struct event_change ch;
	ch.fd = fd;
	ch.old_events = old;
	ch.read_change = ch.write_change = 0;
	if (events & VPK_EV_WRITE)
		ch.write_change = VPK_EVCHANGE_ADD | (events & VPK_EV_ET);
	if (events & VPK_EV_READ)
		ch.read_change = VPK_EVCHANGE_ADD | (events & VPK_EV_ET);

	return epoll_apply_one_change(thiz, thiz->priv, &ch);
}

static int epoll_change_del(vpk_evbase_t *thiz, int fd, short old, short events, void *p)
{
	struct event_change ch;
	ch.fd = fd;
	ch.old_events = old;
	ch.read_change = ch.write_change = 0;
	if (events & VPK_EV_WRITE)
		ch.write_change = VPK_EVCHANGE_DEL;
	if (events & VPK_EV_READ)
		ch.read_change = VPK_EVCHANGE_DEL;

	return epoll_apply_one_change(thiz, thiz->priv, &ch);
}

static int epoll_realloc_events(struct priv_info *priv, int maxs)
{
	int nevents = priv->nevents * 2;
	struct epoll_event *events;

	if (nevents > maxs)
		nevents = maxs;

	events = VPK_REALLOC(priv->events, nevents * sizeof(struct epoll_event));
	if (events) {
		priv->events = events;
		priv->nevents = nevents;
		return 0;
	}

	return -1;
}

static int epoll_dispatch(vpk_evbase_t *thiz, struct timeval *tv)
{
	struct priv_info *priv = thiz->priv;
	struct epoll_event *events = priv->events;
	int i, res;
	long timeout = -1;

	if (tv != NULL) {
		timeout = event_tv_to_msec(tv);
		if (timeout < 0 || timeout > MAX_EPOLL_TIMEOUT_MSEC) {
			/* Linux kernels can wait forever if the timeout is
			 * too big; see comment on MAX_EPOLL_TIMEOUT_MSEC. */
			timeout = MAX_EPOLL_TIMEOUT_MSEC;
		}
	}

	EVBASE_RELEASE_LOCK(thiz, th_base_lock);
	printf("\n\n\n\n");

	res = epoll_wait(priv->epfd, events, priv->nevents, timeout);

	EVBASE_ACQUIRE_LOCK(thiz, th_base_lock);

	if (res == -1) {
		if (errno != EINTR) {
			EVENT_LOGW("epoll_wait");
			return -1;
		}
		return 0;
	}
	EVENT_LOGD(("%s: epoll_wait reports %d", __func__, res));
	for (i = 0; i < res; i++) {
		int what = events[i].events;
		short ev = 0;

		if (what & (EPOLLHUP|EPOLLERR)) {
			ev = VPK_EV_READ | VPK_EV_WRITE;
		} else {
			if (what & EPOLLIN)
				ev |= VPK_EV_READ;
			if (what & EPOLLOUT)
				ev |= VPK_EV_WRITE;
		}

		if (!ev)
			continue;

		evmap_io_active(thiz, events[i].data.fd, ev | VPK_EV_ET);
	}

	if (res == priv->nevents && priv->nevents < EVENT_MAX_NEVENT)
		epoll_realloc_events(priv, EVENT_MAX_NEVENT);

	return 0;
}

static void epoll_dealloc(vpk_evbase_t *thiz)
{
	struct priv_info *priv = thiz->priv;

	evmsg_dealloc(thiz);

	if (priv->events)
		VPK_FREE(priv->events);
	if (priv->epfd >= 0)
		close(priv->epfd);

	memset(priv, 0, sizeof(struct priv_info));
	VPK_FREE(priv);
}

const struct eventop epoll_ops = {
	"epoll",
	epoll_init,
	epoll_change_add,
	epoll_change_del,
	epoll_dispatch,
	epoll_dealloc,
	0
};
