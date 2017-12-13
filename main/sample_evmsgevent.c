/**
 * History:
 * ================================================================
 * 2017-12-08 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "vpk.h"
#include "vpk_events.h"

void *vpk_test1(void* arg);
void *vpk_test2(void* arg);
void *vpk_test3(void* arg);

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

	ret = pthread_create(&pth_test1, NULL, vpk_test1, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test1, &thread_result);
	pthread_join(pth_test3, &thread_result);
	pthread_join(pth_test2, &thread_result);

	return 0;
}

//static int start = 0;
vpk_evbase_t *base = NULL;

vpk_events *events_time;
static struct timeval prev;
int called = 0;

static void test_timer_callback(int fd, short event, void *arg)
{
	//int value = HEART_BEAT_MESSAGE_VALUE;
	double elapsed;
	struct timeval nowtime, difference;

	vpk_gettimeofday(&nowtime, NULL);
	vpk_timersub(&nowtime, &prev, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);
	prev = nowtime;

	LOG_D("test_timer_event, at %d: %.6f seconds elapsed. %p", nowtime.tv_sec, elapsed, &test_timer_callback);

	if (called >= 2)
		vpk_event_free(events_time);

	called++;
}

static int timer_event_add(const char* name)
{
	int ret = -1;
	LOG_D("sample add a timer\n");
	events_time = vpk_event_new(base, 0, VPK_EV_PERSIST, test_timer_callback, NULL);;

	struct timeval tv;
	vpk_timerclear(&tv);
	tv.tv_sec = 5;
	vpk_gettimeofday(&prev, NULL);
	vpk_event_add(events_time, &tv);

	// need free events_time somewhere
	// vpk_event_free(events_time);
	return ret;
}


static int eventq_recv(const char* name)
{
	int ret = -1;
	vpk_event_t alert = {0};
	vpk_eventq_t* eventq = NULL;
	return_val_if_fail(name != NULL, -1);

	eventq = vpk_eventq_open("/test", "a+");
	return_val_if_fail(eventq != NULL, -1);

	LOG_D("recv queue start");
	while (1)
	{
		memset(&alert, 0x00, sizeof(vpk_event_t));
		ret = vpk_eventq_recv(eventq, &alert);
		LOG_I("[%s] ret = %d, recv event key = 0x%x\n", name, ret, alert.alert.keycode);

		if ((ret = vpk_evmsg_notice(alert.alert.keycode)) < 0) {
			LOG_E("vpk_evmsg_notice failed, maybe msg not register callback.");
		}

		sleep(1);
	}

	vpk_eventq_close(eventq);
	vpk_eventq_destroy(eventq);

	return ret;
}


static int start_timer_flag = 0;
static void notice_callback(int fd, short event, void *arg) 
{
	LOG_D("fd(%d) event(0x%02x) in notice_callback.", fd, event);
	start_timer_flag = 1;
}

int msg_event_add(const char* name)
{
	int ret = -1;
	//vpk_events *events_notice = vpk_event_new(base, VPK_KEY_EVENT_SYS_ACC_OFF, VPK_EV_NOTICE|VPK_EV_PERSIST, notice_callback, NULL);;
	vpk_events *events_notice = vpk_event_new(base, KEY2FD(vpk.keys.EVENT_NO_TF_CARD), VPK_EV_NOTICE|VPK_EV_PERSIST, notice_callback, NULL);;

	vpk_event_add(events_notice, NULL);

	//vpk_event_assign(&events_time, base, 0, VPK_EV_PERSIST, test_heart_beat, NULL);

	//struct timeval tv;
	//vpk_timerclear(&tv);
	//tv.tv_sec = 5;
	//vpk_gettimeofday(&prev, NULL);
	//vpk_event_add(events_time, &tv);

	// need free events_time somewhere
	return ret;
}

static void pipe_callback(int fd, short event, void *arg) {
	LOG_D("in pipe_callback.");
}

int msg_event_main(const char* name)
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

void *vpk_test1(void* arg)
{
	while(1)
	{
		LOG_D("test1 thread run.");
		eventq_recv("RECV EVENTQ");
		sleep(1);
	}

	return NULL;
}

void *vpk_test3(void* arg)
{
	sleep(2);
	LOG_D("test3 thread run.");
	char ch = '0';
	printf("\n\n\n");
	LOG_D("please press 's' to start to register a msg event");
	while (ch != 's') {
		scanf("%c", &ch);
	}
	LOG_D("ch = %c", ch);
	printf("\n\n\n");

	msg_event_add(NULL);

	printf("\n\n\n");

	while (!start_timer_flag) {
		sleep(3);
	}

	printf("\n\n\n");
	timer_event_add(NULL);
	printf("\n\n\n");

	return NULL;
}

void *vpk_test2(void* arg)
{
	//sleep(1);
	//LOG_D("start test2 thread!");
	while(1)
	{
		LOG_D("test2 thread run.");
		msg_event_main("main");
		sleep(1);
	}
}
