/**
 * History:
 * ================================================================
 * 2017-11-29 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "vpk.h"
#include "vpk_events.h"

extern void *vpk_test2(void* arg);
extern void *vpk_test3(void* arg);
void *vpk_delay_start(void* arg);

#ifdef USE_ZLOG
#define SAMPLE_ZLOG_FILE_PATH		"."
#define SAMPLE_ZLOG_CONF_FILE		"./zlog.conf"
int sample_zlog_init(int procname)
{
	int rc;
	//zlog_category_t *c;

	if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH)) {
		int ret = 0;
		char tmp[256] = {0};
		vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
		printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
		ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
		printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
	}

	rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
	if (rc)	{
		printf("zlog init failed\n");
		return -1;
	}

	LOG_D("hello, zlog");

	return 0;
}
#endif

int main(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2, pth_test1;

#ifdef USE_ZLOG
	sample_zlog_init(0);
#endif // USE_ZLOG

	vpk_system_init(argc, argv);
	vpk_logging_level_set("DEBUG");


	ret = pthread_create(&pth_test3, NULL, vpk_test3, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test3\' failed");

	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	ret = pthread_create(&pth_test1, NULL, vpk_delay_start, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test3, &thread_result);
	pthread_join(pth_test2, &thread_result);
	pthread_join(pth_test1, &thread_result);

	return 0;
}

static int start = 0;
vpk_evbase_t *base = NULL;

vpk_events *events_time;
static struct timeval prev;

static void test_timer_at_delay(int fd, short event, void *arg)
{
	//int value = HEART_BEAT_MESSAGE_VALUE;
	double elapsed;
	struct timeval nowtime, difference;

	vpk_gettimeofday(&nowtime, NULL);
	vpk_timersub(&nowtime, &prev, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);
	prev = nowtime;

	LOG_D("test_timer_at_delay, at %d: %.6f seconds elapsed.", nowtime.tv_sec, elapsed);

	//vpk_event_free(events_time);
}

int test_event_add_at_delay(const char* name)
{
	int ret = -1;
	LOG_D("sample add another timer\n");
	events_time = vpk_event_new(base, 0, VPK_EV_PERSIST, test_timer_at_delay, NULL);;

	struct timeval tv;
	vpk_timerclear(&tv);
	tv.tv_sec = 5;
	vpk_gettimeofday(&prev, NULL);
	vpk_event_add(events_time, &tv);

	// need free events_time somewhere
	return ret;
}

static void test_heart_beat(int fd, short event, void *arg)
{
	//int value = HEART_BEAT_MESSAGE_VALUE;
	double elapsed;
	struct timeval nowtime, difference;

	vpk_gettimeofday(&nowtime, NULL);
	vpk_timersub(&nowtime, &prev, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);
	prev = nowtime;

	LOG_D("test_heart_beat, at %d: %.6f seconds elapsed.", nowtime.tv_sec, elapsed);

	vpk_event_free(events_time);
	start = 1;
}

int test_event_add(const char* name)
{
	int ret = -1;
	events_time = vpk_event_new(base, 0, VPK_EV_PERSIST, test_heart_beat, NULL);;

	//vpk_event_assign(&events_time, base, 0, VPK_EV_PERSIST, test_heart_beat, NULL);

	struct timeval tv;
	vpk_timerclear(&tv);
	tv.tv_sec = 5;
	vpk_gettimeofday(&prev, NULL);
	vpk_event_add(events_time, &tv);

	// need free events_time somewhere
	return ret;
}

#if 0
int test_event_main(const char* name)
{
	int ret = -1;
	vpk_events events_time;
	return_val_if_fail(name != NULL, -1);

	base = vpk_evbase_create();

	vpk_event_assign(&events_time, base, 0, VPK_EV_PERSIST, test_heart_beat, NULL);

	struct timeval tv;
	vpk_timerclear(&tv);
	tv.tv_sec = 5;
	vpk_gettimeofday(&prev, NULL);
	vpk_event_add(&events_time, &tv);

	vpk_evbase_loop(base, 0);

	return ret;
}
#else
static void pipe_callback(int fd, short event, void *arg) {
	LOG_D("in pipe_callback.");
}
int test_event_main(const char* name)
{
	int ret = -1;
	vpk_events events_pipe;
	return_val_if_fail(name != NULL, -1);

	base = vpk_evbase_create();
	
	int pipe_fd[2];
	pipe(pipe_fd);

	vpk_event_assign(&events_pipe, base, pipe_fd[0], VPK_EV_READ|VPK_EV_PERSIST, pipe_callback, NULL);

	vpk_event_add(&events_pipe, NULL);

	vpk_evbase_loop(base, 0);

	return ret;
}
#endif

void *vpk_delay_start(void* arg)
{

	while (!start) {
		sleep(2);
	}

	sleep(5);
	printf("\n\n-------------------------------\n");
	LOG_D("start vpk_delay_start thread!\n");
	test_event_add_at_delay(NULL);

	return NULL;
}

void *vpk_test3(void* arg)
{
	//sleep(5);
	//LOG_D("start test3 thread!");
	char ch;
	scanf("%c", &ch);
	LOG_D("ch = %c", ch);
	printf("\n\n\n");


	test_event_add(NULL);

	return NULL;
}

void *vpk_test2(void* arg)
{
	//sleep(1);
	//LOG_D("start test2 thread!");
	while(1)
	{
		LOG_D("test2 thread run.");
		test_event_main("main");
		sleep(1);
	}
}
