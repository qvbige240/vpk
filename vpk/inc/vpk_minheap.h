
/**
* History:
* ================================================================
* 2017-08-01 qing.zou created
*
*/

#ifndef VPK_MINHEAP_H
#define VPK_MINHEAP_H

#include <sys/queue.h>		/* tailq */

#include "vpk_typedef.h"
#include "vpk_util.h"

TIMA_BEGIN_DELS

struct vpk_evbase_t;
typedef struct vpk_evbase_t vpk_evbase_t;

typedef struct vpk_events
{
	TAILQ_ENTRY(vpk_events)  ev_active_next;
	TAILQ_ENTRY(vpk_events)  ev_next;

	/* for managing timeouts */
	union {
		//TAILQ_ENTRY(vpk_events) ev_next_with_common_timeout;
		int			min_heap_idx;
	} ev_timeout_pos;
	int				ev_fd;

	vpk_evbase_t*	ev_base;

	union {
		/* used for io events */
		struct {
			TAILQ_ENTRY(vpk_events)  ev_io_next;
			struct timeval ev_timeout;		/** relative time **/
		} ev_io;

		/* used for msg notice events */
		struct {
			TAILQ_ENTRY(vpk_events)  ev_notice_next;
			short	ev_ncalls;
			short	*ev_pncalls;
		} ev_notice;
	} _ev;

	short			ev_events;
	short			ev_result;
	short			ev_flags;
	unsigned char	ev_priority;
	unsigned char	ev_closure;
	struct timeval	ev_timeout;				/** absolute time **/

	/* allows us to adopt for different types of events */
	void (*event_callback)(int fd, short events, void *args);
	void *ev_arg;
} vpk_events;

typedef struct vpk_minheap
{
	vpk_events**	p;
	unsigned		n, a;
} vpk_minheap_t;

static inline void	     minheap_ctor(vpk_minheap_t* s);
static inline void	     minheap_dtor(vpk_minheap_t* s);
static inline void	     minheap_elem_init(vpk_events* e);
static inline int	     minheap_elt_is_top(const vpk_events *e);
static inline int	     minheap_empty(vpk_minheap_t* s);
static inline unsigned	     minheap_size(vpk_minheap_t* s);
static inline vpk_events*  minheap_top(vpk_minheap_t* s);
static inline int	     minheap_reserve(vpk_minheap_t* s, unsigned n);
static inline int	     minheap_push(vpk_minheap_t* s, vpk_events* e);
static inline vpk_events*  minheap_pop(vpk_minheap_t* s);
static inline int	     minheap_adjust(vpk_minheap_t *s, vpk_events* e);
static inline int	     minheap_erase(vpk_minheap_t* s, vpk_events* e);
static inline void	     minheap_shift_up(vpk_minheap_t* s, unsigned hole_index, vpk_events* e);
static inline void	     minheap_shift_up_unconditional(vpk_minheap_t* s, unsigned hole_index, vpk_events* e);
static inline void	     minheap_shift_down(vpk_minheap_t* s, unsigned hole_index, vpk_events* e);

#define minheap_elem_greater(a, b) \
	(vpk_timercmp(&(a)->ev_timeout, &(b)->ev_timeout, >))

void minheap_ctor(vpk_minheap_t* s) { s->p = 0; s->n = 0; s->a = 0; }
void minheap_dtor(vpk_minheap_t* s) { if (s->p) VPK_FREE(s->p); }
void minheap_elem_init(vpk_events* e) { e->ev_timeout_pos.min_heap_idx = -1; }
int minheap_empty(vpk_minheap_t* s) { return 0u == s->n; }
unsigned minheap_size(vpk_minheap_t* s) { return s->n; }
vpk_events* minheap_top(vpk_minheap_t* s) { return s->n ? *s->p : 0; }

int minheap_push(vpk_minheap_t* s, vpk_events* e)
{
	if (minheap_reserve(s, s->n + 1))
		return -1;
	minheap_shift_up(s, s->n++, e);
	return 0;
}

vpk_events* minheap_pop(vpk_minheap_t* s)
{
	if (s->n)
	{
		vpk_events* e = *s->p;
		minheap_shift_down(s, 0u, s->p[--s->n]);
		e->ev_timeout_pos.min_heap_idx = -1;
		return e;
	}
	return 0;
}

int minheap_elt_is_top(const vpk_events *e)
{
	return e->ev_timeout_pos.min_heap_idx == 0;
}

int minheap_erase(vpk_minheap_t* s, vpk_events* e)
{
	if (-1 != e->ev_timeout_pos.min_heap_idx)
	{
		vpk_events *last = s->p[--s->n];
		unsigned parent = (e->ev_timeout_pos.min_heap_idx - 1) / 2;
		/* we replace e with the last element in the heap.  We might need to
		shift it upward if it is less than its parent, or downward if it is
		greater than one or both its children. Since the children are known
		to be less than the parent, it can't need to shift both up and
		down. */
		if (e->ev_timeout_pos.min_heap_idx > 0 && minheap_elem_greater(s->p[parent], last))
			minheap_shift_up_unconditional(s, e->ev_timeout_pos.min_heap_idx, last);
		else
			minheap_shift_down(s, e->ev_timeout_pos.min_heap_idx, last);
		e->ev_timeout_pos.min_heap_idx = -1;
		return 0;
	}
	return -1;
}

int minheap_adjust(vpk_minheap_t *s, vpk_events *e)
{
	if (-1 == e->ev_timeout_pos.min_heap_idx) {
		return minheap_push(s, e);
	} else {
		unsigned parent = (e->ev_timeout_pos.min_heap_idx - 1) / 2;
		/* The position of e has changed; we shift it up or down
		* as needed.  We can't need to do both. */
		if (e->ev_timeout_pos.min_heap_idx > 0 && minheap_elem_greater(s->p[parent], e))
			minheap_shift_up_unconditional(s, e->ev_timeout_pos.min_heap_idx, e);
		else
			minheap_shift_down(s, e->ev_timeout_pos.min_heap_idx, e);
		return 0;
	}
}

int minheap_reserve(vpk_minheap_t* s, unsigned n)
{
	if (s->a < n)
	{
		vpk_events** p;
		unsigned a = s->a ? s->a * 2 : 8;
		if (a < n)
			a = n;
		if (!(p = (vpk_events**)VPK_REALLOC(s->p, a * sizeof *p)))
			return -1;
		s->p = p;
		s->a = a;
	}
	return 0;
}

void minheap_shift_up_unconditional(vpk_minheap_t* s, unsigned hole_index, vpk_events* e)
{
	unsigned parent = (hole_index - 1) / 2;
	do
	{
		(s->p[hole_index] = s->p[parent])->ev_timeout_pos.min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	} while (hole_index && minheap_elem_greater(s->p[parent], e));
	(s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

void minheap_shift_up(vpk_minheap_t* s, unsigned hole_index, vpk_events* e)
{
	unsigned parent = (hole_index - 1) / 2;
	while (hole_index && minheap_elem_greater(s->p[parent], e))
	{
		(s->p[hole_index] = s->p[parent])->ev_timeout_pos.min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	(s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

void minheap_shift_down(vpk_minheap_t* s, unsigned hole_index, vpk_events* e)
{
	unsigned min_child = 2 * (hole_index + 1);
	while (min_child <= s->n)
	{
		min_child -= min_child == s->n || minheap_elem_greater(s->p[min_child], s->p[min_child - 1]);
		if (!(minheap_elem_greater(e, s->p[min_child])))
			break;
		(s->p[hole_index] = s->p[min_child])->ev_timeout_pos.min_heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}
	(s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

TIMA_END_DELS

#endif /* VPK_MINHEAP_H */
