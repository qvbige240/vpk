/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "vpk.h"

// extern void *vpk_test2(void* arg);
extern void *vpk_test3(void* arg);


//vpk_eventq_t* eventq = NULL;

int main(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	//vpk_logging_level_set("DEBUG");

	//eventq = vpk_eventq_open();

	ret = pthread_create(&pth_test3, NULL, vpk_test3, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test3\' failed");

// 	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
// 	if (ret != 0)
// 		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test3, &thread_result);
// 	pthread_join(pth_test2, &thread_result);

	return 0;
}

int test_event_post(const char* name)
{
	int ret = -1;
	vpk_event_t alert1 = {0};
	vpk_event_t alert2 = {0};
	vpk_eventq_t* eventq = NULL;
	return_val_if_fail(name != NULL, -1);

	eventq = vpk_eventq_open("/test", "w");
	return_val_if_fail(eventq != NULL, -1);

	LOG_D("send queue start");
	alert1.type = vpk.events.ALERT;
	alert1.alert.keycode = vpk.keys.EVENT_NO_TF_CARD;

	alert2.type = vpk.events.ALERT;
	alert2.alert.keycode = vpk.keys.EVENT_TF_CARD_WRITE_PROTECT;
	while (1)
	{
		ret = vpk_eventq_post(eventq, &alert1);
		LOG_I("[%s] ret = %d, post alert key = 0x%x\n", name, ret, alert1.alert.keycode);
		sleep(10);
// 
// 		ret = vpk_eventq_post(eventq, &alert2);
// 		LOG_I("[%s] ret = %d, post alert key = 0x%x", name, ret, alert2.alert.keycode);
// 		sleep(10);
	}

	vpk_eventq_close(eventq);
	vpk_eventq_destroy(eventq);

	return ret;
}
// 
// int test_event_recv(const char* name)
// {
// 	int ret = -1;
// 	vpk_event_t alert = {0};
// 	vpk_eventq_t* eventq = NULL;
// 	return_val_if_fail(name != NULL, -1);
// 
// 	eventq = vpk_eventq_open("/test", "a+");
// 	return_val_if_fail(eventq != NULL, -1);
// 
// 	LOG_D("recv queue start");
// 	while (1)
// 	{
// 		memset(&alert, 0x00, sizeof(vpk_event_t));
// 		ret = vpk_eventq_recv(eventq, &alert);
// 		LOG_I("[%s] ret = %d, recv alert key = 0x%x\n", name, ret, alert.alert.keycode);
// 		sleep(1);
// 	}
// 
// 	vpk_eventq_close(eventq);
// 	vpk_eventq_destroy(eventq);
// 
// 	return ret;
// }

void *vpk_test3(void* arg)
{
	sleep(5);
	LOG_D("start test3 thread!");
	while(1)
	{
		LOG_D("test3 thread run.\n");
		test_event_post("POST EVENT");
		sleep(1);
	}
}
// 
// void *vpk_test2(void* arg)
// {
// 	sleep(1);
// 	LOG_D("start test2 thread!");
// 	while(1)
// 	{
// 		LOG_D("test2 thread run.");
// 		test_event_recv("RECV EVENT");
// 		sleep(1);
// 	}
// }
