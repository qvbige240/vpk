/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
// #include <pthread.h>

#include "vpk.h"
#include "jansson.h"

typedef struct VpkGps {
	char latitude[32];
	char longitude[32];
	char speed[32];
	char angle[32];
}VpkGps;

int vpk_parse(VpkGps *gps, json_t* root_object, const char *item, ...)
{
	va_list va;
	char *ptr = (char *)item;
	char *info = gps->latitude;

	double  jstring = 0.0;
	json_t* jobject = NULL;
	//json_t* root_object = NULL;

	LOG_D("va args ptr: %s ", ptr);
	va_start(va, item);
	while (ptr != NULL)
	{
		jobject = json_object_get(root_object, ptr);
		return_val_if_fail(jobject != NULL, -1);

		jstring = (double)json_real_value(jobject);
		LOG_D("jstring %s: %lf", ptr, jstring);

		sprintf(info, "%lf", jstring);
		LOG_I("info.member: %lf", jstring);
		ptr = va_arg(va, char*);
		if (ptr)
		{
			info += sizeof(gps->latitude);
			LOG_D("va args ptr: %s ", ptr);
		}
	}
	LOG_D("info: %s %s %s %s", gps->latitude, gps->longitude, gps->speed, gps->angle);
	va_end(va);

	return 0;
}

int vpk_gps_parse() 
{
	json_t* jobject = NULL;
	json_t* root_object = NULL;

	double jstring = 0.0;
	VpkGps gps_info;
	char *str = "{\"Latitude\":0.100011,\"Longitude\":0.200022,\"Speed\":0.300033,\"Angle\":0.400044}";

	LOG_D("parse str: %s\n", str);
	memset(&gps_info, 0, sizeof(gps_info));

	root_object = json_loads(str, 0, NULL);
	return_val_if_fail(root_object != NULL, -1);

	vpk_parse(&gps_info, root_object, "Latitude", "Longitude", "Speed", "Angle", NULL);


	jobject = json_object_get(root_object, "Latitude");
	return_val_if_fail(jobject != NULL, -1);

	jstring = (double)json_real_value(jobject);
	LOG_D("jstring Latitude: %lf", jstring);

	sprintf(gps_info.latitude, "%lf", jstring);
	LOG_I("gps_info.latitude: %lf", jstring);


	jobject = json_object_get(root_object, "Longitude");
	return_val_if_fail(jobject != NULL, -1);

	jstring = (double)json_real_value(jobject);
	LOG_D("jstring Longitude: %lf", jstring);

	sprintf(gps_info.longitude, "%lf", jstring);
	LOG_I("gps_info.longitude: %lf", jstring);


	jobject = json_object_get(root_object, "Speed");
	return_val_if_fail(jobject != NULL, -1);

	jstring = (double)json_real_value(jobject);
	LOG_D("jstring Speed: %lf", jstring);

	sprintf(gps_info.speed, "%lf", jstring);
	LOG_I("gps_info.speed: %lf", jstring);


	jobject = json_object_get(root_object, "Angle");
	return_val_if_fail(jobject != NULL, -1);

	jstring = (double)json_real_value(jobject);
	LOG_D("jstring Angle: %lf", jstring);

	sprintf(gps_info.angle, "%lf", jstring);
	LOG_I("gps_info.angle: %lf", jstring);


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

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

int SetTimer()
{
	struct itimerval interval;
	interval.it_interval.tv_sec = 0;
	interval.it_interval.tv_usec = 1;
	interval.it_value.tv_sec = 1;
	interval.it_value.tv_usec = 0;

	if (::setitimer(ITIMER_REAL, &interval, NULL)) 
	{
		return -1;
	}    
	return 0;
}

void Handler(int nSigNo)
{
	printf("ALARM signal!\n");
	char *p = (char*)malloc(rand() % 40960);
	free(p);
}

int main()
{
	SetTimer();

	signal(SIGALRM, Handler);

	while (1)
	{
		char *p = (char*)malloc(rand() % 40960);
		free(p);
	}
	return 0;
}
*/