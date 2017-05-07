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
// #include "vpk_logging.h"
// #include "vpk_system.h"

//extern void *vpk_pppd_thread(void* arg);
extern void *vpk_test2(void* arg);
extern void *vpk_test3(void* arg);

// #define LOG_D(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_I(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_W(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_E(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_F(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)


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


int test_nvtuctrl(const char* name)
{
	int ret = -1;
	char recv_buf[256] = {0};
	char cmd[256] = {0};
	char cmd_2[256] = {0};
	char cmd_3[256] = {0};
	vpk_nvtuctrl_t* nvtuctrl = NULL;
	return_val_if_fail(name != NULL, -1);

	nvtuctrl = vpk_nvtuctrl_create(name);
	return_val_if_fail(nvtuctrl != NULL, -1);

	ret = vpk_nvtuctrl_open(nvtuctrl);
	if (ret < 0)
	{
		LOG_E("open nvtuctrl \'%s\' failed.", name);
	}

	snprintf(cmd, sizeof(cmd), "ucustom -gpsinfo");
	snprintf(cmd_2, sizeof(cmd_2), "ucustom -snapshot 1");
	snprintf(cmd_3, sizeof(cmd_3), "ucustom -getdevinfo ");

	while (1)
	{
		ret = vpk_nvtuctrl_write(nvtuctrl, cmd, sizeof(cmd), recv_buf, sizeof(recv_buf), 0);
		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
		sleep(1);

		ret = vpk_nvtuctrl_write(nvtuctrl, cmd_2, sizeof(cmd_2), recv_buf, sizeof(recv_buf), 0);
		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
		sleep(1);

		ret = vpk_nvtuctrl_write(nvtuctrl, cmd_3, sizeof(cmd_3), recv_buf, sizeof(recv_buf), 0);
		LOG_I("[%s]devinfo ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
		sleep(1);
	}

	vpk_nvtuctrl_close(nvtuctrl);
	vpk_nvtuctrl_destroy(nvtuctrl);

	return ret;
}

void *vpk_test3(void* arg)
{

	LOG_D("start test3 thread!");
	while(1)
	{
		LOG_D("test3 thread run.");
		//test_nvtuctrl1("/dev/ttyAMA3");
		test_nvtuctrl("GPS1");
		sleep(2);
	}
}

void *vpk_test2(void* arg)
{
	LOG_D("start test2 thread!");
	while(1)
	{
		LOG_D("test2 thread run.");
		test_nvtuctrl("GPS2");
		sleep(2);
	}
}
