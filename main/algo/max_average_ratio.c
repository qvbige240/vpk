

// double compare
// double b = sin(M_PI / 6.0);
// if (fabs(b - 0.5) < DBL_EPSILON)
// float compare
// float b = sin(M_PI / 6.0);
// if (fabs(b - 0.5) < 0.000001)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#define elem_object() \
	int heap_idx

typedef struct heap_elem
{
	elem_object();
} heap_elem_t;

typedef int (*heap_elem_cmp)(void *a, void *b);

typedef struct vpk_heap_
{
	heap_elem_t **p;
	unsigned n, a;
	heap_elem_cmp cmp;
} vpk_heap_t;

static inline void vpk_heap_ctor(vpk_heap_t *s, heap_elem_cmp compare);
static inline void vpk_heap_dtor(vpk_heap_t *s);
static inline void vpk_heap_elem_init(heap_elem_t *e);
static inline int vpk_heap_elt_is_top(const heap_elem_t *e);
static inline int vpk_heap_empty(vpk_heap_t *s);
static inline unsigned vpk_heap_size(vpk_heap_t *s);
static inline heap_elem_t *vpk_heap_top(vpk_heap_t *s);
static inline int vpk_heap_reserve(vpk_heap_t *s, unsigned n);
static inline int vpk_heap_push(vpk_heap_t *s, heap_elem_t *e);
static inline heap_elem_t *vpk_heap_pop(vpk_heap_t *s);
static inline int vpk_heap_adjust(vpk_heap_t *s, heap_elem_t *e);
static inline int vpk_heap_erase(vpk_heap_t *s, heap_elem_t *e);
static inline void vpk_heap_shift_up(vpk_heap_t *s, unsigned hole_index, heap_elem_t *e);
static inline void vpk_heap_shift_up_unconditional(vpk_heap_t *s, unsigned hole_index, heap_elem_t *e);
static inline void vpk_heap_shift_down(vpk_heap_t *s, unsigned hole_index, heap_elem_t *e);

#define vpk_heap_elem_compare(a, b) \
	(s->cmp(a, b) > 0)

void vpk_heap_ctor(vpk_heap_t *s, heap_elem_cmp compare)
{
	s->p = 0;
	s->n = 0;
	s->a = 0;
	s->cmp = compare;
}
void vpk_heap_dtor(vpk_heap_t *s)
{
	if (s->p)
		free(s->p);
}
void vpk_heap_elem_init(heap_elem_t *e) { e->heap_idx = -1; }
int vpk_heap_empty(vpk_heap_t *s) { return 0u == s->n; }
unsigned vpk_heap_size(vpk_heap_t *s) { return s->n; }
heap_elem_t *vpk_heap_top(vpk_heap_t *s) { return s->n ? *s->p : 0; }

int vpk_heap_push(vpk_heap_t *s, heap_elem_t *e)
{
	if (vpk_heap_reserve(s, s->n + 1))
		return -1;
	vpk_heap_shift_up(s, s->n++, e);
	return 0;
}

heap_elem_t *vpk_heap_pop(vpk_heap_t *s)
{
	if (s->n)
	{
		heap_elem_t *e = *s->p;
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

int vpk_heap_erase(vpk_heap_t *s, heap_elem_t *e)
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
	if (-1 == e->heap_idx)
	{
		return vpk_heap_push(s, e);
	}
	else
	{
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

int vpk_heap_reserve(vpk_heap_t *s, unsigned n)
{
	if (s->a < n)
	{
		heap_elem_t **p;
		unsigned a = s->a ? s->a * 2 : 8;
		if (a < n)
			a = n;
		if (!(p = (heap_elem_t **)realloc(s->p, a * sizeof(heap_elem_t*))))
			return -1;
		s->p = p;
		s->a = a;
	}
	return 0;
}

void vpk_heap_shift_up_unconditional(vpk_heap_t *s, unsigned hole_index, heap_elem_t *e)
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

void vpk_heap_shift_up(vpk_heap_t *s, unsigned hole_index, heap_elem_t *e)
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

void vpk_heap_shift_down(vpk_heap_t *s, unsigned hole_index, heap_elem_t *e)
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

typedef struct _test_element_t
{
	elem_object();

	double delta;
	int pass;
	int total;
} test_element_t;

// #define FLT_EPSILON 0.000001

static int element_compare_diff(void *a, void *b)
{
	test_element_t *s1 = (test_element_t *)a;
	test_element_t *s2 = (test_element_t *)b;

	// s1->delta = (double)(s1->pass + 1) / (s1->total + 1) - (double)s1->pass / s1->total;
	// s2->delta = (double)(s2->pass + 1) / (s2->total + 1) - (double)s2->pass / s2->total;
	if (s1->delta - s2->delta > DBL_EPSILON)
		return 0;
	else
		return 1;
}

static double maxAverageRatio(int **classes, int classesSize, int *classesColSize, int extraStudents)
{
	double max = 0.0;
	int i = 0;

	int col_size = *classesColSize;
	int n = classesSize;
	int extra_students = extraStudents;

	printf("countof array: %d\n", n);

	vpk_heap_t heap;
	vpk_heap_ctor(&heap, element_compare_diff);
	test_element_t *start = calloc(n, sizeof(test_element_t));

	for (i = 0; i < n; i++)
	{
		int pass = *((int *)classes + col_size * i + 0);
		int total = *((int *)classes + col_size * i + 1);
		// int pass = classes[i][0];
		// int total = classes[i][1];

		max += (double)pass / total;

		double delta = (double)(pass + 1) / (total + 1) - (double)pass / total;

		// push to minheap
		test_element_t *e = (test_element_t *)((char *)start + i * sizeof(test_element_t));
		// test_element_t e = {0};
		vpk_heap_elem_init((heap_elem_t *)e);
		e->delta = delta;
		e->pass = pass + 1;
		e->total = total + 1;
		vpk_heap_push(&heap, (heap_elem_t *)e);
		printf("added element %.6lf(%d:%d)\n", e->delta, e->pass, e->total);
	}

	printf("\n");

	test_element_t *e;
	for (i = 0; i < extra_students; i++)
	{
		e = (test_element_t *)vpk_heap_pop(&heap);

		max += e->delta;

		e->delta = (double)(e->pass + 1) / (e->total + 1) - (double)e->pass / e->total;

		e->pass += 1;
		e->total += 1;

		vpk_heap_push(&heap, (heap_elem_t *)e);
		printf("added element %.6lf(%d:%d)\n", e->delta, e->pass, e->total);
	}

	free(start);
	vpk_heap_dtor(&heap);

	return max / n;
}

int main(int argc, char *argv[])
{
	int extra_students = 2;
	int classes[3][2] = {{1, 2}, {3, 5}, {2, 2}};

	// int n = _countof(classes);
	int n = sizeof(classes) / sizeof classes[0];
	int col = sizeof(classes[0]) / sizeof(classes[0][0]);

	double max_ratio = maxAverageRatio((int **)classes, n, &col, extra_students);
	printf("max average ratio: %.6f\n", max_ratio);
	return 0;
}
