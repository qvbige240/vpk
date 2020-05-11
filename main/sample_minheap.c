/**
 * History:
 * ================================================================
 * 2017-08-01 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>

#include "vpk.h"
#include "vpk_minheap.h"

#if defined(_X86_)
#else
#endif

//#include <errno.h>

static int use_monotonic;
static void detect_monotonic(void)
{
	struct timespec	ts;
	static int use_monotonic_initialized = 0;

	if (use_monotonic_initialized)
		return;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		use_monotonic = 1;
	LOG_D("ts: %d %ld", ts.tv_sec, ts.tv_nsec);

	use_monotonic_initialized = 1;
}

#define CLOCK_SYNC_INTERVAL -1
static time_t last_updated_clock_diff = 0;
static struct timeval tv_clock_diff;
static int gettime(struct timeval *tp)
{
	//if (base->tv_cache.tv_sec) {
	//	*tp = base->tv_cache;
	//	return (0);
	//}

	if (use_monotonic) {
		struct timespec	ts;

		if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
			return (-1);

		tp->tv_sec = ts.tv_sec;
		tp->tv_usec = ts.tv_nsec / 1000;
		if (last_updated_clock_diff + CLOCK_SYNC_INTERVAL < ts.tv_sec) {
				struct timeval tv;
				vpk_gettimeofday(&tv,NULL);
				vpk_timersub(&tv, tp, &tv_clock_diff);
				last_updated_clock_diff = ts.tv_sec;
				LOG_D("real time: %d %d, clock mono: %d %ld, diff: %d %d",
					tv.tv_sec, tv.tv_usec, ts.tv_sec, ts.tv_nsec, tv_clock_diff.tv_sec, tv_clock_diff.tv_usec);
		}

		return (0);
	}

	return (vpk_gettimeofday(tp, NULL));
}

int minheap_test(void)
{	
	int array[] = {11, 40, 7, 3, 88, 1, 19, 5, 20, 5, 23, 20}; 
	vpk_minheap_t timeheap;

	minheap_ctor(&timeheap);

	int i = 0;
	int n = _countof(array);
	//int s = sizeof(vpk_events);
	vpk_events* start = calloc(n, sizeof(vpk_events));
	for (i = 0; i < n; i++)
	{
		//vpk_events* e = (vpk_events*)(start + i * sizeof(vpk_events));
		vpk_events* e = (vpk_events*)((char*)start + i * sizeof(vpk_events));
		minheap_elem_init(e);
		e->ev_timeout.tv_sec = array[i];
		minheap_push(&timeheap, e);
		printf("added events %2ld(%d:%d)\n", e->ev_timeout.tv_sec, n, i);
	}

	vpk_events* ev;
	int size = minheap_size(&timeheap);
	printf("[%d]index: \n", size);
	for (i = 0; i < size; i++)
		printf(" %2d", timeheap.p[i]->ev_timeout_pos.min_heap_idx);
	printf("\n");

	printf("[%d]value: \n", size);
#if 1
	while ( (ev = minheap_top(&timeheap)) != NULL)
	{
		//LOG_D(" %d", ev->ev_timeout);

		if (ev->ev_timeout.tv_sec == 5) {
			printf(" (%ld)", ev->ev_timeout.tv_sec);
			ev->ev_timeout.tv_sec = 100;
			minheap_adjust(&timeheap, ev);
		} else {
			printf(" %2ld", ev->ev_timeout.tv_sec);
			minheap_erase(&timeheap, ev);
		}
	}
#else
	while(minheap_size(&timeheap) > 0)
	{
		ev = minheap_pop(&timeheap);
		printf(" %2ld", ev->ev_timeout.tv_sec);
	}
#endif
	printf("\n");

	if (start)
		free(start);

	if (!minheap_empty(&timeheap)) {
		LOG_W("min heap not empty!");
		return -1;
	}

	minheap_dtor(&timeheap);
	return 0;
}

#include "vpk_heap.h"
typedef struct test_element
{
	elem_object();

	time_t		ctime;
	char		name[256];
} test_element_t;

int element_compare(void *a, void *b)
{
	test_element_t *e1 = (test_element_t*)a;
	test_element_t *e2 = (test_element_t*)b;
	if (e1->ctime > e2->ctime)
		return 1;
	else
		return 0;
}

int heap_test(void)
{
	int array[] = {11, 40, 7, 3, 88, 1, 19, 5, 20, 5, 23, 20}; 
	vpk_heap_t timeheap;

	vpk_heap_ctor(&timeheap, element_compare);

	int i = 0;
	int n = _countof(array);
	//int s = sizeof(test_element_t);
	test_element_t* start = calloc(n, sizeof(test_element_t));
	for (i = 0; i < n; i++)
	{
		test_element_t* e = (test_element_t*)((char*)start + i * sizeof(test_element_t));
		vpk_heap_elem_init((heap_elem_t*)e);
		e->ctime = array[i];
		vpk_heap_push(&timeheap, (heap_elem_t*)e);
		printf("added element %2ld(%d:%d)\n", e->ctime, n, i);
	}

	test_element_t* e;
	int size = vpk_heap_size(&timeheap);
	printf("[%d]index: \n", size);
	for (i = 0; i < size; i++)
		printf(" %ld[%d]", ((test_element_t*)timeheap.p[i])->ctime, ((test_element_t*)timeheap.p[i])->heap_idx);
		//printf(" %2d", timeheap.p[i]->heap_idx);
	printf("\n");

	printf("[%d]value: \n", size);
#if 1
	while ( (e = (test_element_t*)vpk_heap_top(&timeheap)) != NULL)
	{
		//LOG_D(" %d", ev->ev_timeout);

		if (e->ctime == 5) {
			printf(" (%ld)", e->ctime);
			e->ctime = 100;
			vpk_heap_adjust(&timeheap, (heap_elem_t*)e);
		} else {
			printf(" %2ld", e->ctime);
			vpk_heap_erase(&timeheap, (heap_elem_t*)e);
		}
	}
#else
	while(vpk_heap_size(&timeheap) > 0)
	{
		e = (test_element_t*)vpk_heap_pop(&timeheap);
		printf(" %2ld", e->ctime);
	}
#endif
	printf("\n");

	if (start)
		free(start);

	if (!vpk_heap_empty(&timeheap)) {
		LOG_W("min heap not empty!");
		return -1;
	}

	vpk_heap_dtor(&timeheap);
	return 0;
}

int minheap_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	//char* pathname = "./ipc";
	//int proj_id = 0x10;
	char* type = "minheap";
	//if (argc > 1)
	//{
	//	type = argv[1];
	//}

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 't':
				type = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s", type);

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	if (strcasecmp(type, "minheap") == 0) {
		minheap_test();
	} else if (strcasecmp(type, "heap") == 0) {
		heap_test();
	} else {
		detect_monotonic();
		struct timeval now;
		gettime(&now);
		LOG_D("now: %d %d\n", now.tv_sec, now.tv_usec);

		int n = 6, i = 1;
		int s = sizeof(struct timeval);
		struct timeval *start = calloc(n, sizeof(struct timeval ));
		struct timeval* e = (struct timeval*)((char*)start + i * sizeof(struct timeval));
		printf("start: %p, add: 0x%x(%d), pos: %p	(char*)+\n", start, s, s, e);
		e = (struct timeval*)((void*)start + i * sizeof(struct timeval));
		printf("start: %p, add: 0x%x(%d), pos: %p	(void*)+\n", start, s, s, e);
		e = (struct timeval*)((struct timeval*)start + i * sizeof(struct timeval));
		printf("start: %p, add: 0x%x(%d), pos: %p	(struct timeval*)+\n", start, s, s, e);
		e = (struct timeval*)(start + i * sizeof(struct timeval));
		printf("start: %p, add: 0x%x(%d), pos: %p	+\n", start, s, s, e);
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);		//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
