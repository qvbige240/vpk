/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>
// #include <pthread.h>

#include "vpk.h"
#include "jansson.h"

static int jsonf_item_set(char* file, char* key, int value)
{
	json_t* json_root = NULL;
	if (vpk_mmap_exist(file)) {
		json_root = json_load_file(file, 0, NULL);
	}
	else {
		LOG_I("file not exist, and new json_root");
		json_root = json_object();
	}

	if (json_root)
	{
		json_t* jobject;

		jobject = json_object_get(json_root, key);
		if (!jobject)
		{
			LOG_W("jobject not exist, and new json object.");
			json_object_set(json_root, key, json_integer(value));

			char* update_dump = json_dumps(json_root, 0);				/* to buffer */

			json_dump_file(json_root, file, 0);		/* to file */

			LOG_D("update_dump setting:\n%s\n", update_dump);

			free(update_dump);
		}
		else
		{
			int curr_val = json_integer_value(jobject);
			if (curr_val != value)
			{
				json_object_set(json_root, key, json_integer(value));

				char* update_dump = json_dumps(json_root, 0);				/* to buffer */

				json_dump_file(json_root, file, 0);		/* to file */

				LOG_D("update_dump setting:\n%s\n", update_dump);

				free(update_dump);
			}
			else
			{
				LOG_D("no value change at \'%s\'", key);
			}
		}

		json_decref(json_root);
	}

	return 0;
}

static int json_item_set(char* data, char* key, int value)
{
	json_t* json_root = NULL;
	json_root = json_loads(data, 0, NULL);
	if (json_root)
	{
		json_t* jobject;

		jobject = json_object_get(json_root, key);
		if (!jobject)
			goto json_parse_end;

		int curr_val = json_integer_value(jobject);
		if (curr_val != value)
		{
			json_object_set(json_root, key, json_integer(value));

			char* update_dump = json_dumps(json_root, 0);				/* to buffer */

			json_dump_file(json_root, "./dev_setting_pc.json", 0);		/* to file */

			LOG_D("update_dump setting:\n%s\n", update_dump);

			free(update_dump);
		}


		json_decref(json_root);
		return 0;

json_parse_end:
		LOG_E("\n ======= json_object_get error!!!! =======\n");
		json_decref(json_root);
	}

	return -1;
}

int test_findstr(const char* str)
{
	int r = 0;
	char buf[64];
	char* string = "ucustom -picsizeset MENUMOCK 1";

	char s1[64], s2[64];
	char *s = "1try234delete5";
	sscanf(s, "1%[^2]234%[^5]", s1, s2);
	LOG_D("s1: \'%s\', s2: \'%s\'", s1, s2);

	char* p = strstr(string, "MENUMOCK");
	if (p != NULL)
	{
		r = sscanf(string, "ucustom -%s MENUMOCK\n", buf);
		p = strstr(buf, "set");
		if (p)
		{
			int pos = p - buf;
			buf[pos] = '\0';
			LOG_D("buf: \'%s\'", buf);
			p = strstr(string, "MENUMOCK");
			if (p)
			{
				p = p + strlen("MENUMOCK");
				int value = atoi(p);

				LOG_D("p = %s, value(atoi) = %d\n", p, value);
			}
		}

		r = r;

	}

	return 0;
}

void test_null(void)
{
	//char buffer[64] = {0};
	char* str = NULL;
	int ret = -1;
	if (str && strlen(str))
	 	ret = strlen(str);
	//memcpy(buffer, NULL, 0);

	LOG_D("ret len: %d", ret);
	//ret = strcasecmp("1", NULL);
	LOG_D("ret cmp: %d", ret);
	//const char* desc = "hello";
	const char* desc = NULL;
	if (!desc) desc = "Device Exception Parse Error!";
	LOG_D("desc: %s", desc);
}

typedef struct _MediaListTest 
{
	char		file[NAME_MAX+1];
} MediaListTest ;

typedef struct _MediaResTest 
{
	int			count;
	MediaListTest	media[4];
} MediaResTest;

void test_struct(void)
{
	MediaResTest res = {0};
	int size = sizeof(res.media);
	LOG_D("struct size = %d", size);
}

int string_main(int argc, char *argv[])
{
// 	int ret = 0;

// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	char* menus = "{\"photo_resolution\": 0, \"video_resolution\": 0, \"video_time\": 0, \"vibration_sensor\": 0, \"parking_mode\": 0, \"off_voltage\": 0, \"record_sound\": 0, \"HDR\": 0, \"time_watermark\": 0, \"upgrade\": 0, \"switch_machine\": 0, \"car_warning\": 0, \"driving\": 0, \"abnormal_equipment\": 0, \"format_tf\": 0, \"factory_reset\": 0, \"version\": \"0\"}";
	char* data = malloc(strlen(menus)+1);
	strcpy(data, menus);
	//LOG_D("data: %s\n", data);
	test_struct();
	int i = atoi(" 2");
	LOG_D("i(atoi) = %d\n", i);
	test_null();
	test_findstr(NULL);
	jsonf_item_set("./setting_pc.json", "photo_resolution", 1);
	jsonf_item_set("./setting_pc.json", "photo_resolution", 2);
	
	json_item_set(data, "photo_resolution", 1);

	return 0;
}

