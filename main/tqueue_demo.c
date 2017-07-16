#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/time.h>

#include "vpk.h"

struct csq_data
{
	//	char data[MAX_CSQ_DATA];
	int data;
	int  len;
};

vpk_tqueue *my_queue = NULL;

void *vpk_mycsq_read_thread(void *arg);
void *vpk_mycsq_write_thread(void *arg);

int vpk_tqueue_test(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_csq_read, pth_csq_write;

	my_queue = vpk_tqueue_create(sizeof(struct csq_data), 10);

	ret = pthread_create(&pth_csq_read, NULL, vpk_mycsq_read_thread, (void*)NULL);
	 	if (ret != 0)
	 		LOG_E("create thread \'vpk_mycsq_read_thread\' failed");

	ret = pthread_create(&pth_csq_write, NULL, vpk_mycsq_write_thread, (void*)NULL);
	 	if (ret != 0)
	 		LOG_E("create thread \'vpk_mycsq_write_thread\' failed");

	pthread_join(pth_csq_read, &thread_result);
	pthread_join(pth_csq_write, &thread_result);

	return 0;
}

void *vpk_mycsq_read_thread(void *arg)
{
	struct csq_data csq = {0};

	LOG_I("vpk_mycsq_read_thread start\n");
	while (1)
	{
		while (vpk_tqueue_read(my_queue, (void *)&csq, 2) == 0)
		{
			//printf("read csq: %d\n", csq.data);
			LOG_I("read csq: %d\n", csq.data);
			sleep(1);
		}
		sleep(2);
	}	
}

void *vpk_mycsq_write_thread(void *arg)
{
	int ret = 0;
	struct csq_data csq = {0};

	csq.data = 1234;
	csq.len = 4;

	LOG_I("vpk_mycsq_write_thread start\n");
	while (1)
	{
		ret = vpk_tqueue_write(my_queue, (void *)&csq);
		LOG_D("write csq %d, ret = %d", csq.data, ret);

		csq.data++;
		sleep(1);
	}
}

/* tailq */
#include <sys/queue.h>

struct task_event {
	TAILQ_ENTRY(task_event) next;
	TAILQ_ENTRY(task_event) active_next;

	//struct {								
	//	struct task_event *tqe_next;	/* next element */			
	//	struct task_event **tqe_prev;	/* address of previous next element */	
	//} next;

	const char *name;
};

TAILQ_HEAD(task_event_queue, task_event);
//struct task_event_queue {								
//	struct task_event *tqh_first;	/* first element */			
//	struct task_event **tqh_last;	/* addr of last next element */		
//};

struct task_info {
	struct task_event_queue activequeue;

	int event_active_cnt;
};

static struct task_info tinfo;

static void task_queue_insert(struct task_info* tasks, struct task_event* ev)
{
	struct task_event_queue* activeq = &tasks->activequeue;
	tasks->event_active_cnt++;
	TAILQ_INSERT_TAIL(activeq, ev, active_next);
}

static void task_queue_remove(struct task_info* tasks, struct task_event* ev)
{
	struct task_event_queue* activeq = &tasks->activequeue;
	tasks->event_active_cnt--;
	TAILQ_REMOVE(activeq, ev, active_next);
}

static int task_queue_foreach(struct task_info* tasks)
{
	struct task_event* ev;
	struct task_event_queue* activeq = &tasks->activequeue;

	TAILQ_FOREACH(ev, activeq, active_next)
	{
		LOG_D("name: %s", ev->name);
	}

	return 0;
}

void tailq_test(void)
{
	TAILQ_INIT(&tinfo.activequeue);

	char* name1 = "data1";
	char* name2 = "king";
	char* name3 = "qing";

	struct task_event* ev1 = malloc(sizeof(struct task_event));
	ev1->name = name1;

	struct task_event* ev2 = malloc(sizeof(struct task_event));
	ev2->name = name2;

	struct task_event* ev3 = malloc(sizeof(struct task_event));
	ev3->name = name3;

	task_queue_insert(&tinfo, ev1);
	task_queue_insert(&tinfo, ev2);
	task_queue_insert(&tinfo, ev3);

	LOG_D("foreach tail queue: size(%d)", tinfo.event_active_cnt);
	task_queue_foreach(&tinfo);

	struct task_event *entry;

	while ((entry = TAILQ_FIRST(&tinfo.activequeue)) != NULL) {
		//TAILQ_REMOVE(&tinfo.activequeue, entry, active_next);
		task_queue_remove(&tinfo, entry);
	}

	LOG_D("foreach tail queue: size(%d)", tinfo.event_active_cnt);
	task_queue_foreach(&tinfo);
}

int queue_main(int argc, char *argv[])
{
	char* type = "tqueue";
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
		switch(o) {
		case 't':
			type = optarg;
			break;
		default:break;
		}
	}

	LOG_D("queue type = %s", type);


	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	if (strcasecmp(type, "tqueue") == 0) {
		vpk_tqueue_test(argc, argv);
	} else if (strcasecmp(type, "tailq") == 0) {
		tailq_test();
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);
	//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);


	return 0;
}
