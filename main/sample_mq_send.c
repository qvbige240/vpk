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
#include <getopt.h>

#include "vpk.h"

// extern void *vpk_test2(void* arg);
extern void *vpk_test3(void* arg);


//vpk_eventq_t* eventq = NULL;
#define HEXSTR_PARSE(ptr)	\
	((vpk_hex_to_int(*(ptr)) << 12) | (vpk_hex_to_int(*((ptr)+1)) << 8) \
	| (vpk_hex_to_int(*((ptr)+2)) << 4) | vpk_hex_to_int(*((ptr)+3)))

int mqsend_main(int argc, char *argv[])
{
	int ret = 0, type = 0, keycode = 0;
	void* thread_result;
	pthread_t pth_test3;

	//vpk_system_init(argc, argv);
	//vpk_logging_level_set("DEBUG");

	//eventq = vpk_eventq_open();
	
	vpk_event_t evt = {0};

	if (type == 0) {
		evt.type = vpk.events.ABNORMAL;
		evt.abnormal.keycode = vpk.keys.EVENT_NO_TF_CARD;
	} else if (type == 1) {
		evt.type = vpk.events.ABNORMAL;
		evt.abnormal.keycode = vpk.keys.EVENT_NO_TF_CARD;
	} else if (type == 2) {
		evt.type = vpk.events.ALERT;
		evt.alert.keycode = vpk.keys.EVENT_CAR_CRASH_WARNING;
	} else if (type == 3) {
		evt.type = vpk.events.ALERT;
		evt.alert.keycode = vpk.keys.EVENT_PARKING_CRASH_WARNING;
	} else if (type == 4) {
		evt.type = vpk.events.NOTICE;
		evt.notice.keycode = vpk.keys.EVENT_FIRMWARE_DOWNLOAD;
	}

	//LOG_I("type = %d, evt.type = %d, keycode = 0x%x\n", type, evt.type, evt.alert.keycode);

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
	//LOG_D("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);

		switch(o) {
			case 't':
				type = atoi(optarg);
				LOG_D("type: %d", type);
				break;
			case 'k':
				keycode = HEXSTR_PARSE(optarg);
				LOG_D("keycode: %x", keycode);
				break;
			default:
				break;
		}
	}

	LOG_I("type = %d, keycode = 0x%x", type, keycode);
	evt.type = type;
	evt.alert.keycode = keycode;
	LOG_I("evt.type = %d, keycode = 0x%x\n", evt.type, evt.alert.keycode);

/*
	if (argc > 1) {
		type = atoi(argv[1]);

		if (type == 1) {
			evt.type = vpk.events.ABNORMAL;
			evt.abnormal.keycode = vpk.keys.EVENT_NO_TF_CARD;
		} else if (type == 2) {
			evt.type = vpk.events.ALERT;
			evt.alert.keycode = vpk.keys.EVENT_CAR_CRASH_WARNING;
		} else if (type == 3) {
			evt.type = vpk.events.ALERT;
			evt.alert.keycode = vpk.keys.EVENT_PARKING_CRASH_WARNING;
		} else if (type == 4) {
			evt.type = vpk.events.NOTICE;
			evt.notice.keycode = vpk.keys.EVENT_FIRMWARE_DOWNLOAD;
		}

	} else
	if (argc > 2) {
		type = atoi(argv[1]);
		keycode = HEXSTR_PARSE(argv[2]);
		
		if (type == 1) {
			evt.type = vpk.events.ABNORMAL;
			//evt.abnormal.keycode = vpk.keys.EVENT_NO_TF_CARD;
			evt.abnormal.keycode = keycode;
		} else if (type == 2) {
			evt.type = vpk.events.ALERT;
			evt.alert.keycode = keycode;
		} else if (type == 3) {
			evt.type = vpk.events.ALERT;
			evt.alert.keycode = keycode;
		} else if (type == 4) {
			evt.type = vpk.events.NOTICE;
			evt.notice.keycode = keycode;
		}
	}*/

	ret = pthread_create(&pth_test3, NULL, vpk_test3, (void*)&evt);
	if (ret != 0)
		LOG_E("create thread \'vpk_test3\' failed");

// 	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
// 	if (ret != 0)
// 		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test3, &thread_result);
// 	pthread_join(pth_test2, &thread_result);

	return 0;
}

int test_event_post(const char* name, void* args)
{
	int ret = -1;
	vpk_event_t evt = {0};
	vpk_eventq_t* eventq = NULL;
	return_val_if_fail(name != NULL, -1);

	eventq = vpk_eventq_open("/test", "w");
	return_val_if_fail(eventq != NULL, -1);

	evt = *(vpk_event_t*)args;
	LOG_D("send queue start");

	while (1)
	{
		ret = vpk_eventq_post(eventq, &evt);
		LOG_I("[%s] ret = %d, post key = 0x%x\n", name, ret, evt.alert.keycode);

		sleep(16);
	}

	vpk_eventq_close(eventq);
	vpk_eventq_destroy(eventq);

	return ret;
}
//int test_event_post(const char* name, void* args)
//{
//	int ret = -1;
//	vpk_event_t alert0 = {0};
//	vpk_event_t alert1 = {0};
//	vpk_event_t alert2 = {0};
//	vpk_eventq_t* eventq = NULL;
//	return_val_if_fail(name != NULL, -1);
//
//	eventq = vpk_eventq_open("/test", "w");
//	return_val_if_fail(eventq != NULL, -1);
//
//	LOG_D("send queue start");
//	alert0.type = vpk.events.ABNORMAL;
//	alert0.abnormal.keycode = vpk.keys.EVENT_NO_TF_CARD;
//
//	alert1.type = vpk.events.ALERT;
//	alert1.alert.keycode = vpk.keys.EVENT_CAR_CRASH_WARNING;
//
//	alert2.type = vpk.events.ALERT;
//	alert2.alert.keycode = vpk.keys.EVENT_PARKING_CRASH_WARNING;
//
//	while (1)
//	{
//		if (*((int*)type) == 1) 
//		{
//			ret = vpk_eventq_post(eventq, &alert1);
//			LOG_I("[%s] ret = %d, post alert key = 0x%x\n", name, ret, alert1.alert.keycode);
//		} 
//		else if (*((int*)type) == 2) 
//		{
//			ret = vpk_eventq_post(eventq, &alert2);
//			LOG_I("[%s] ret = %d, post alert key = 0x%x", name, ret, alert2.alert.keycode);
//		}
//		else
//		{
//			ret = vpk_eventq_post(eventq, &alert0);
//			LOG_I("[%s] ret = %d, post abnormal key = 0x%x\n", name, ret, alert0.abnormal.keycode);
//		}
//
//		sleep(10);
//	}
//
//	vpk_eventq_close(eventq);
//	vpk_eventq_destroy(eventq);
//
//	return ret;
//}

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
	LOG_D("start test3 thread!");
	sleep(1);
	while(1)
	{
		//LOG_D("test3 thread run.\n");
		test_event_post("POST EVENT", arg);
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
