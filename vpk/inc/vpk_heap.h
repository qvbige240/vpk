
/**
* History:
* ================================================================
* 2017-08-01 qing.zou created, min heap
*
*/

#ifndef VPK_HEAP_H
#define VPK_HEAP_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

#define elem_object()	\
	int heap_idx

typedef struct heap_elem
{
	elem_object();
} heap_elem_t;

typedef int (*heap_elem_cmp)(void *a, void *b);

typedef struct vpk_heap_
{
	heap_elem_t		**p;
	unsigned		n, a;
	heap_elem_cmp	cmp;
//	int				(*cmp)(void *a, void *b);
//	int				(*heap_idx_set)(void *e, int index);
//	int				(*heap_idx_get)(void *e);
} vpk_heap_t;

static inline void	     vpk_heap_ctor(vpk_heap_t* s, heap_elem_cmp compare);
static inline void	     vpk_heap_dtor(vpk_heap_t* s);
static inline void	     vpk_heap_elem_init(heap_elem_t* e);
static inline int	     vpk_heap_elt_is_top(const heap_elem_t *e);
static inline int	     vpk_heap_empty(vpk_heap_t* s);
static inline unsigned	     vpk_heap_size(vpk_heap_t* s);
static inline heap_elem_t*  vpk_heap_top(vpk_heap_t* s);
static inline int	     vpk_heap_reserve(vpk_heap_t* s, unsigned n);
static inline int	     vpk_heap_push(vpk_heap_t* s, heap_elem_t* e);
static inline heap_elem_t*  vpk_heap_pop(vpk_heap_t* s);
static inline int	     vpk_heap_adjust(vpk_heap_t *s, heap_elem_t* e);
static inline int	     vpk_heap_erase(vpk_heap_t* s, heap_elem_t* e);
static inline void	     vpk_heap_shift_up(vpk_heap_t* s, unsigned hole_index, heap_elem_t* e);
static inline void	     vpk_heap_shift_up_unconditional(vpk_heap_t* s, unsigned hole_index, heap_elem_t* e);
static inline void	     vpk_heap_shift_down(vpk_heap_t* s, unsigned hole_index, heap_elem_t* e);

#define vpk_heap_elem_compare(a, b) \
	(s->cmp(a, b) > 0)

void vpk_heap_ctor(vpk_heap_t* s, heap_elem_cmp compare) { s->p = 0; s->n = 0; s->a = 0; s->cmp = compare; }
void vpk_heap_dtor(vpk_heap_t* s) { if (s->p) VPK_FREE(s->p); }
void vpk_heap_elem_init(heap_elem_t* e) { e->heap_idx = -1; }
int vpk_heap_empty(vpk_heap_t* s) { return 0u == s->n; }
unsigned vpk_heap_size(vpk_heap_t* s) { return s->n; }
heap_elem_t* vpk_heap_top(vpk_heap_t* s) { return s->n ? *s->p : 0; }

int vpk_heap_push(vpk_heap_t* s, heap_elem_t* e)
{
	if (vpk_heap_reserve(s, s->n + 1))
		return -1;
	vpk_heap_shift_up(s, s->n++, e);
	return 0;
}

heap_elem_t* vpk_heap_pop(vpk_heap_t* s)
{
	if (s->n)
	{
		heap_elem_t* e = *s->p;
		vpk_heap_shift_down(s, 0u, s->p[--s->n]);
		e->heap_idx = -1;
		return e;
	}
	return 0;
}

int vpk_heap_elt_is_top(const heap_elem_t *e)
{
	return e->heap_idx == 0;
}

int vpk_heap_erase(vpk_heap_t* s, heap_elem_t* e)
{
	if (-1 != e->heap_idx)
	{
		heap_elem_t *last = s->p[--s->n];
		unsigned parent = (e->heap_idx - 1) / 2;
		/* we replace e with the last element in the heap.  We might need to
		shift it upward if it is less than its parent, or downward if it is
		greater than one or both its children. Since the children are known
		to be less than the parent, it can't need to shift both up and
		down. */
		if (e->heap_idx > 0 && vpk_heap_elem_compare(s->p[parent], last))
			vpk_heap_shift_up_unconditional(s, e->heap_idx, last);
		else
			vpk_heap_shift_down(s, e->heap_idx, last);
		e->heap_idx = -1;
		return 0;
	}
	return -1;
}

int vpk_heap_adjust(vpk_heap_t *s, heap_elem_t *e)
{
	if (-1 == e->heap_idx) {
		return vpk_heap_push(s, e);
	} else {
		unsigned parent = (e->heap_idx - 1) / 2;
		/* The position of e has changed; we shift it up or down
		* as needed.  We can't need to do both. */
		if (e->heap_idx > 0 && vpk_heap_elem_compare(s->p[parent], e))
			vpk_heap_shift_up_unconditional(s, e->heap_idx, e);
		else
			vpk_heap_shift_down(s, e->heap_idx, e);
		return 0;
	}
}

int vpk_heap_reserve(vpk_heap_t* s, unsigned n)
{
	if (s->a < n)
	{
		heap_elem_t** p;
		unsigned a = s->a ? s->a * 2 : 8;
		if (a < n)
			a = n;
		if (!(p = (heap_elem_t**)VPK_REALLOC(s->p, a * sizeof *p)))
			return -1;
		s->p = p;
		s->a = a;
	}
	return 0;
}

void vpk_heap_shift_up_unconditional(vpk_heap_t* s, unsigned hole_index, heap_elem_t* e)
{
	unsigned parent = (hole_index - 1) / 2;
	do
	{
		(s->p[hole_index] = s->p[parent])->heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	} while (hole_index && vpk_heap_elem_compare(s->p[parent], e));
	(s->p[hole_index] = e)->heap_idx = hole_index;
}

void vpk_heap_shift_up(vpk_heap_t* s, unsigned hole_index, heap_elem_t* e)
{
	unsigned parent = (hole_index - 1) / 2;
	while (hole_index && vpk_heap_elem_compare(s->p[parent], e))
	{
		(s->p[hole_index] = s->p[parent])->heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	(s->p[hole_index] = e)->heap_idx = hole_index;
}

void vpk_heap_shift_down(vpk_heap_t* s, unsigned hole_index, heap_elem_t* e)
{
	unsigned min_child = 2 * (hole_index + 1);
	while (min_child <= s->n)
	{
		min_child -= min_child == s->n || vpk_heap_elem_compare(s->p[min_child], s->p[min_child - 1]);
		if (!(vpk_heap_elem_compare(e, s->p[min_child])))
			break;
		(s->p[hole_index] = s->p[min_child])->heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}
	(s->p[hole_index] = e)->heap_idx = hole_index;
}

TIMA_END_DELS

#endif /* VPK_HEAP_H */
