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

static int firmware_json_parse(char* data, int len)
{
	json_t* json_root = NULL;
	json_root = json_loads(data, 0, NULL);
	if (json_root)
	{
		json_t* jobject;
		json_t* firmware_array;
		firmware_array = json_object_get(json_root, "appUpgraderInfoDto");
		if (!firmware_array)
			goto firmware_parse_end;

		jobject = json_object_get(firmware_array, "md5");
		if (!jobject)
			goto firmware_parse_end;

		char* md5 = (char*)json_string_value(jobject);
		if (!md5)
		{
			LOG_E("\n ======= get json string error!!!! =======\n");
			goto firmware_parse_end;
		}

		jobject = json_object_get(firmware_array, "url");
		if (!jobject)
			goto firmware_parse_end;

		char* url = (char*)json_string_value(jobject);
		if (!url)
		{
			LOG_E("\n ======= get json string error!!!! =======\n");
			goto firmware_parse_end;
		}

		jobject = json_object_get(firmware_array, "version");
		if (!jobject)
			goto firmware_parse_end;

		char* version = (char*)json_string_value(jobject);
		if (!version)
		{
			LOG_E("\n ======= get json string error!!!! =======\n");
			goto firmware_parse_end;
		}
		LOG_D("version: %s, md5: %s, url: %s", version, md5, url);


		jobject = json_object_get(firmware_array, "appName");
		if (!jobject)
			goto firmware_parse_end;

		char* name = (char*)json_string_value(jobject);
		if (!name)
		{
			LOG_E("\n ======= get json string error!!!! =======\n");
			goto firmware_parse_end;
		}
		LOG_D("app name: %s", name);

// 		strcpy(thiz->domain, domains);

		json_decref(json_root);
		return 0;

firmware_parse_end:
		LOG_E("\n ======= json_object_get error!!!! =======\n");
		json_decref(json_root);
	}

	return -1;
}

int json_main(int argc, char *argv[])
{
// 	int ret = 0;
// 	void* thread_result;
// 	pthread_t pth_test3, pth_test2;


// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	char* data = "{\"returnSuccess\": true,\"returnErrCode\": null,\"returnErrMsg\": null,\"appUpgraderInfoDto\": {		\
				\"version\": \"3.03.100\",			\
				\"url\": \"http://7xl1at.com2.z0.glb.qiniucdn.com/121.mp4\",		\
				\"appName\": \"升级固件包\",		\
				\"deployDesc\": \"升级固件包\",		\
				\"md5\": \"asdfewfd\",						\
				\"pkgSize\": 43333333}}";

	LOG_D("data: %s\n", data);

	//vpk_gps_parse();
	firmware_json_parse(data, 0);

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

