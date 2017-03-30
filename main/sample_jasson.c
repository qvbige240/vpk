/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <pthread.h>

#include "vpk.h"
#include "jansson.h"

typedef struct vpk_gps_t {
	char latitude[32];
	char longitude[32];
	char speed[32];
	char angle[32];
}vpk_gps_t;

int vpk_gps_parse() 
{
	json_t* jobject = NULL;
	json_t* root_object = NULL;

	double jstring = 0.0;
	vpk_gps_t gps_info;
	char *str = "{\"Latitude\":0.000000,\"Longitude\":0.000000,\"Speed\":0.000000,\"Angle\":0.000000}";

	LOG_D("parse str: %s\n", str);
	memset(&gps_info, 0, sizeof(gps_info));

	root_object = json_loads(str, 0, NULL);
	return_val_if_fail(root_object != NULL, -1);

	jobject = json_object_get(root_object, "Latitude");
	return_val_if_fail(jobject != NULL, -1);

	jstring = (double)json_real_value(jobject);
	LOG_D("jstring: %lf\n", jstring);

	//strcpy(gps_info.latitude, jstring);
	if (root_object)
		json_decref(root_object);

	return 0;
}

int main(int argc, char *argv[])
{
// 	int ret = 0;
// 	void* thread_result;
// 	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	vpk_logging_level_set("DEBUG");

	vpk_gps_parse();

// 	ret = pthread_create(&pth_test3, NULL, vpk_test3, (void*)NULL);
// 	if (ret != 0)
// 		LOG_E("create thread \'vpk_test3\' failed");
// 
// 	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
// 	if (ret != 0)
// 		LOG_E("create thread \'vpk_test2\' failed");
// 
// 	pthread_join(pth_test3, &thread_result);
// 	pthread_join(pth_test2, &thread_result);

	return 0;
}

/*
int test_nvtuctrl(const char* name)
{
	int ret = -1;
	char recv_buf[256] = {0};
	char cmd[256] = {0};
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

	while (1)
	{
		ret = vpk_nvtuctrl_write(nvtuctrl, cmd, sizeof(cmd), recv_buf, sizeof(recv_buf), 0);
		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
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
		sleep(1);
	}
}

void *vpk_test2(void* arg)
{
	LOG_D("start test2 thread!");
	while(1)
	{
		LOG_D("test2 thread run.");
		test_nvtuctrl("GPS2");
		sleep(1);
	}
}
*/