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
#include <semaphore.h>

#include "vpk.h"
// #include "vpk_logging.h"
// #include "vpk_system.h"

extern void *vpk_test2(void* arg);
extern void *vpk_test3(void* arg);

typedef struct _PrivInfo
{
	pthread_mutex_t sample_mutex;
	sem_t			sample_sem;
}PrivInfo;

PrivInfo info;

int main(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	vpk_logging_level_set("DEBUG");


	ret = pthread_create(&pth_test3, NULL, vpk_test3, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test3\' failed");

	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
	if (ret != 0)
		LOG_E("create thread \'vpk_test2\' failed");

	pthread_join(pth_test3, &thread_result);
	pthread_join(pth_test2, &thread_result);

	return 0;
}



//int test_nvtuctrl(const char* name)
//{
//	int ret = -1;
//	char recv_buf[256] = {0};
//	char cmd[256] = {0};
//	char cmd_2[256] = {0};
//	vpk_nvtuctrl_t* nvtuctrl = NULL;
//	return_val_if_fail(name != NULL, -1);
//
//	nvtuctrl = vpk_nvtuctrl_create(name);
//	return_val_if_fail(nvtuctrl != NULL, -1);
//
//	ret = vpk_nvtuctrl_open(nvtuctrl);
//	if (ret < 0)
//	{
//		LOG_E("open nvtuctrl \'%s\' failed.", name);
//	}
//
//	snprintf(cmd, sizeof(cmd), "ucustom -gpsinfo");
//	snprintf(cmd_2, sizeof(cmd_2), "ucustom -snapshot 1");
//
//	while (1)
//	{
//		ret = vpk_nvtuctrl_write(nvtuctrl, cmd, sizeof(cmd), recv_buf, sizeof(recv_buf), 0);
//		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
//		sleep(1);
//
//		ret = vpk_nvtuctrl_write(nvtuctrl, cmd_2, sizeof(cmd_2), recv_buf, sizeof(recv_buf), 0);
//		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
//		sleep(1);
//	}
//
//	vpk_nvtuctrl_close(nvtuctrl);
//	vpk_nvtuctrl_destroy(nvtuctrl);
//
//	return ret;
//}

void *vpk_test3(void* arg)
{
	int ret = 0, recv_cnt = 0;
	PrivInfo* thiz = &info;
	ret = pthread_mutex_init(&thiz->sample_mutex, NULL);
	if (ret != 0) 
	{
		LOG_E("sample_mutex init error.");
		return NULL;
	}
	ret = sem_init(&thiz->sample_sem, 0, 0);
	if (ret != 0)
	{
		pthread_mutex_destroy(&thiz->sample_mutex);
		LOG_E("sample_sem init error.");
		return NULL;
	}

	LOG_D("start test3 thread!");
	while(1)
	{
		LOG_D("test3 thread run.");

		ret = sem_wait(&thiz->sample_sem);
		if (ret == 0)
		{
			recv_cnt++;
			LOG_I("receive a sample_sem.");
			pthread_mutex_lock(&thiz->sample_mutex);
			//test_nvtuctrl("GPS1");
			LOG_D("recv_cnt = %d, sleep 5\n\n", recv_cnt);
			sleep(5);
			pthread_mutex_unlock(&thiz->sample_mutex);
		}
		else
		{
			LOG_E("snap_sem wait error!");
			sleep(2);
		}

		//sleep(1);
	}
}

static INLINE int sample_sem_post(PrivInfo *thiz)
{
	int ret = -1;
	return_val_if_fail(thiz != NULL, -1);

	ret = sem_post(&thiz->sample_sem);
	if (ret != 0)
		LOG_E("sample_sem post failed!");

	return ret;
}

void *vpk_test2(void* arg)
{
	sleep(3);
	LOG_D("start test2 thread!");
	int send_cnt = 0;
	PrivInfo* thiz = &info;
	while(1)
	{
		LOG_D("test2 thread run.");
		//pthread_mutex_lock(&thiz->sample_mutex);
		send_cnt++;
		LOG_D("send_cnt = %d, send a semaphore!\n", send_cnt);
		sample_sem_post(thiz);
		if (send_cnt == 6)
			break;
		//pthread_mutex_unlock(&thiz->sample_mutex);
		sleep(1);
	}

	LOG_D("==== end test2 thread! ====\n\n");

	return NULL;
}
