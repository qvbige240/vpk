/**
 * History:
 * ================================================================
 * 2017-05-28 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/time.h>

#include "vpk.h"

static pthread_mutex_t mutex;
static pthread_cond_t cond;
static int size = -1;

static void *vpk_test2(void* arg)
{
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	while(1)
	{
		pthread_mutex_lock(&mutex);
		while (size > -1) {
			LOG_D("pthread_cond_wait");
			pthread_cond_wait(&cond, &mutex);
		}

		//size++;
		size = 1;
		LOG_D("[send]size = %d", size);

		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

static void *vpk_test1(void* arg)
{
	char ch = '0';

	while(1)
	{
		//usleep(100000);
		ch = '0';
		LOG_D("please press 's' to start a condition\n\n\n");

		while (ch != 's') {
			scanf("%c", &ch);
		}
#if 0
		//pthread_mutex_lock(&mutex);
		size--;
		LOG_D("[recv]size = %d", size);
		pthread_cond_broadcast(&cond);
		//pthread_mutex_unlock(&mutex);
#else
		if (size > -1) {
			size = -1;
			LOG_D("[recv]size = %d", size);
			pthread_cond_broadcast(&cond);
		}
#endif
	}

	return NULL;
}

static int condition_test(void)
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test2, pth_test1;

	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	ret = pthread_create(&pth_test1, NULL, vpk_test1, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test1, &thread_result);
	pthread_join(pth_test2, &thread_result);

	return 0;
}

int condition_main(int argc, char *argv[])
{
 	//int ret = 0;
 
 	//vpk_system_init(argc, argv);
 	//vpk_logging_level_set("DEBUG");

	char* type = "condition";
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

	if (strcasecmp(type, "condition") == 0) {
		condition_test();
	} else if (strcasecmp(type, "sem") == 0) {

	} else {
		condition_test();
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);		//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
