/**
 * History:
 * ================================================================
 * 2017-09-01 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

#include "vpk.h"

#if defined(TIMA_PROTOCOL)

#include "timapi/tima_eventq.h"
#include "timapi/tima_command.h"

#include "jansson.h"

#define BUFFER_SIZE			EVENT_QUEUE_MSG_LEN_MAX

const char *str1 = "{\"event\":\"3001\",\"msg\":null}";
const char *str_2001 = "{\"event\":\"2001\",\"msg\":{\"id\":0,\"key\":\"2001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"2017_a.TS\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"2017_b.TS\\\"}]\"}}";
const char *str_2002 = "{\"event\":\"2002\",\"msg\":{\"id\":0,\"key\":\"2002\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":0,\\\"file\\\":\\\"2017_collision.JPG\\\"},{\\\"camera\\\":1,\\\"type\\\":0,\\\"file\\\":\\\"P70625-161747.jpg\\\"}]\"}}";
const char *str_5001 = "{\"event\":\"5001\",\"msg\":{\"id\":0,\"key\":\"5001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"youngforyou.mp4\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"test_2016_1201_080020_FR.MP4\\\"}]\"}}";
const char *str_5002 = "{\"event\":\"5002\",\"msg\":{\"id\":0,\"key\":\"5002\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":0,\\\"file\\\":\\\"test_2017_0409_171946_RE.JPG\\\"},{\\\"camera\\\":1,\\\"type\\\":0,\\\"file\\\":\\\"P70625-161747.jpg\\\"}]\"}}";

//const char *str2 = "{\"event\":\"5001\",\"msg\":{\"value\":\"{\\\"type\\\":0}\"}}";
//const char *str = "{\"event\":\"5001\",\"msg\":{\"id\":0,\"key\":\"5001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"/xxx/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/01.mp4\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"/xxx/02.mp4\\\"}]\"}}";

static void event_mock_json_create(int fd, char *code)
{
	int ret = 0;
	json_t* json_root = json_object();

	json_object_set_new(json_root, "event", json_string(code));
	json_object_set_new(json_root, "msg", json_null());
	char* event_dump = json_dumps(json_root, 0);

	ret = tima_mqueue_post(fd, (void*)event_dump);
	LOG_D("event_dump: %s, ret = %d", event_dump, ret);

	free(event_dump);
	json_decref(json_root);
}

static int tima_event_mock(int fd, char *code)
{
	int ret = 0;
	int keycode = HEX_PARSE_DIGIT_STR(code);

	if (strlen(code) != 4 && (keycode > 0x9999 || keycode < 0x1001))
		LOG_W("keycode format is wrong, usage: 1001~9999 which is defined");

	if (keycode == 0x2001) {
		ret = tima_mqueue_post(fd, (void*)str_2001);
		printf("post: %s, ret = %d\n", str_2001, ret);
	} else if (keycode == 0x2002) {
		ret = tima_mqueue_post(fd, (void*)str_2002);
		printf("post: %s, ret = %d\n", str_2002, ret);
	} else if (keycode == 0x5001) {
		ret = tima_mqueue_post(fd, (void*)str_5001);
		printf("post: %s, ret = %d\n", str_5001, ret);
	} else if (keycode == 0x5002) {
		ret = tima_mqueue_post(fd, (void*)str_5002);
		printf("post: %s, ret = %d\n", str_5002, ret);
	} else {
		event_mock_json_create(fd, code);
	}
	return 0;
}

static void *event_post(void *name)
{
	char key_buffer[256] = {0};

	//int fd = tima_mqueue_open(EVENT_MQ_PATH, 'a');
	int fd = tima_mqueue_open();
	
	while (1) {
		memset(key_buffer, 0x00, sizeof(key_buffer));
		printf("Enter keycode(2001/3001 in 1001~9999) to send: \n");
		scanf("%s", key_buffer);
		tima_event_mock(fd, key_buffer);

		sleep(1);
	}

	return name;
}

static sem_t mock_sem;
static char* reply_code = NULL;
static void *command_reply_async(void *name)
{
	int ret = 0;

	//int fd = tima_mqueue_open(EVENT_MQ_PATH, 'a');
	int fd = tima_mqueue_open();
	ret = sem_init(&mock_sem, 0, 0);
	if (ret != 0) {
		LOG_E("mock_sem init error.");
		return NULL;
	}

	while (1) {	

		ret = sem_wait(&mock_sem);
		if (ret == 0) {
			sleep(3);
			tima_event_mock(fd, reply_code);
		} else {
			LOG_E("snap_sem wait error!");
			sleep(1);
		}

	}

	return name;
}

static INLINE void command_reply_semaphore(void)
{
	int ret = sem_post(&mock_sem);
	if (ret != 0)
		LOG_E("mock_sem sem post failed!");
}

static int setting_item_get(const char* file, char* key, int* value)
{
	int ret = 0;
	json_t* json_root = NULL;
	if (vpk_mmap_exist(file)) {
		json_root = json_load_file(file, 0, NULL);
	}
	else {
		LOG_W("file not exist, and new json_root");
		json_root = json_object();
	}

	if (json_root)
	{
		json_t* jobject = json_object_get(json_root, key);
		if (!jobject)
		{
			LOG_W("jobject not exist, and return default 0.");
			*value = 0;
			ret = -1;
		}
		else
		{
			*value = json_integer_value(jobject);
		}

		json_decref(json_root);
	}

	return ret;
}
static int setting_item_write(const char* file, char* key, int value)
{
	json_t* json_root = NULL;
	if (vpk_mmap_exist(file)) {
		json_root = json_load_file(file, 0, NULL);
	}
	else {
		LOG_W("file not exist, and new json_root");
		json_root = json_object();
	}

	if (json_root)
	{
		json_t* jobject = json_object_get(json_root, key);
		if (!jobject)
		{
			LOG_W("jobject not exist, and new json object.");
			json_object_set(json_root, key, json_integer(value));
			json_dump_file(json_root, file, 0);						/* to file */

			char* update_dump = json_dumps(json_root, 0);			/* to buffer */
			LOG_D("update_dump setting:\n%s\n", update_dump);
			free(update_dump);
		}
		else
		{
			int curr_val = json_integer_value(jobject);
			if (curr_val != value)
			{
				json_object_set(json_root, key, json_integer(value));
				json_dump_file(json_root, file, 0);						/* to file */

				char* update_dump = json_dumps(json_root, 0);			/* to buffer */
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

static char gps_str[256] = {0};
static int tima_command_mock(int fd, char *recv_buff)
{
	int rc = 0;
	int ret = -1;
	char *buf = recv_buff;

	if (buf)
	{
		char *p = NULL;
		//GPS
		p = strstr(buf, "-gpsinfo");
		if (p != NULL) 
		{
			//char *str = "{\"Latitude\":30.657300,\"Longitude\":104.065855,\"Speed\":0.300003,\"Angle\":0.400004}";
			//char *str = "{\"Latitude\":0.100001,\"Longitude\":0.200002,\"Speed\":0.300003,\"Angle\":0.400004}";
			//memcpy(thiz->data_buff, str, strlen(str));

			#define DEVICE_GPS_INFO_FILE	"dev_gps_pc.json"
			if (!strlen(gps_str)) {
				ret = vpk_mmap_exist(DEVICE_GPS_INFO_FILE);
				VpkMmap* mmap = vpk_mmap_create(DEVICE_GPS_INFO_FILE, 0, -1);
				char* str = vpk_mmap_data(mmap);
				strncpy(gps_str, str, sizeof(gps_str));
				LOG_D("read GPS json: %s", gps_str);
				vpk_mmap_destroy(mmap);
			}
			rc = tima_command_send(fd, gps_str, strlen(gps_str), 0);

			return 0;
		}
		// SNAP
		p = strstr(buf, "-snapshot");
		if (p != NULL)
		{
			//const char *str = "{\"event\":\"5002\",\"msg\":{\"id\":0,\"key\":\"5002\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":0,\\\"file\\\":\\\"P70625-161747.jpg\\\"},{\\\"camera\\\":1,\\\"type\\\":0,\\\"file\\\":\\\"02.JPG\\\"}]\"}}";
			const char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			printf("rc = %d, reply: %s\n", rc, str);
			reply_code = "5002";
			command_reply_semaphore();
			return 0;
		}
		// VIDEO
		p = strstr(buf, "-movierec");
		if (p != NULL)
		{
			//const char *str = "{\"event\":\"5001\",\"msg\":{\"id\":0,\"key\":\"5001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"2017_a.TS\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"2017_b.TS\\\"}]\"}}";
			const char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			printf("rc = %d, reply: %s\n", rc, str);
			reply_code = "5001";
			command_reply_semaphore();
			return 0;
		}

		p = strstr(buf, "-getdevinfo");
		if (p != NULL)
		{
			#define DEVICE_RELEASE_INFO_FILE	"dev_info_pc.json"
			ret = vpk_mmap_exist(DEVICE_RELEASE_INFO_FILE);
			VpkMmap* mmap = vpk_mmap_create(DEVICE_RELEASE_INFO_FILE, 0, -1);
			char* str = vpk_mmap_data(mmap);
			rc = tima_command_send(fd, str, strlen(str), 0);
			vpk_mmap_destroy(mmap);
			return 0;
		}
		p = strstr(buf, "-fwdownload");
		if (p != NULL)
		{
			const char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			printf("rc = %d, reply: %s\n", rc, str);
			reply_code = "1051";
			command_reply_semaphore();
			return 0;
		}
		p = strstr(buf, "-fwupdate");	/** tima -fwupdate   1 **/
		if (p != NULL)
		{
			const char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			printf("rc = %d, reply: %s\n", rc, str);
			if (strstr(p, "1")) {
				reply_code = "1052";
				command_reply_semaphore();
			}
			return 0;
		}
		p = strstr(buf, "-qrcodeshow");
		if (p != NULL)
		{
			char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "-setiotid");
		if (p != NULL)
		{
			char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "-setiotlicence");
		if (p != NULL)
		{
			char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "-poweroff");
		if (p != NULL)
		{
			char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "-removebind");
		if (p != NULL)
		{
			char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "-versionget");
		if (p != NULL)
		{
			char *str = "K90.20170808.01";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "-wifiphraseget");
		if (p != NULL)
		{
			char *str = "DECAI_,%s";
			rc = tima_command_send(fd, str, strlen(str), 0);
			return 0;
		}
		p = strstr(buf, "MENUMOCK");		/* menus setting mock */
		if (p != NULL)
		{
#define DEVICE_MENUS_INFO_FILE	"menu_setting_pc.json"
			int value = 0;
			char *key = NULL;
			char tmp[256] = {0};
			char reply_buff[32] = {0};
			int r = sscanf(buf, "tima -%s MENUMOCK\n", tmp);
			if (r) {
				p = strstr(tmp, "set");
				if (p)
				{
					int pos = p - tmp;
					tmp[pos] = '\0';
					key = tmp;
					p = strstr(buf, "MENUMOCK");
					if (p)
					{
						p = p + strlen("MENUMOCK");
						value = atoi(p);
						LOG_D("set key = %s, value(atoi) = %d\n", key, value);
						setting_item_write(DEVICE_MENUS_INFO_FILE, key, value);
						sprintf(reply_buff, "%d", value);
					}
				}
				p = strstr(tmp, "get");
				if (p)
				{
					int pos = p - tmp;
					tmp[pos] = '\0';
					key = tmp;
					setting_item_get(DEVICE_MENUS_INFO_FILE, key, &value);
					LOG_D("get key = %s, value = %d\n", key, value);
					sprintf(reply_buff, "%d", value);
				}
				else
				{
					char *str = "0";
					memcpy(reply_buff, str, strlen(str));
				}
			} else {
				char *str = "0";
				memcpy(reply_buff, str, strlen(str));
			}
			rc = tima_command_send(fd, reply_buff, strlen(reply_buff), 0);

			return 0;
		}		
		else
		{
			char *str = "unkonwn command!!!";
			rc = tima_command_send(fd, str, strlen(str), 0);
		}

	}
	return ret;
}

static void *command_mock(void *name)
{
	int rc;
	int count = 0;
	char recvbuf[256];

	int fd = tima_command_open();
	if (fd < 0) {
		printf("error\n");
		return NULL;
	}
	printf("server fd = %d\n", fd);

	while (1) {
		memset(recvbuf, 0, sizeof(recvbuf));
		rc = tima_command_recv(fd, recvbuf, sizeof(recvbuf), 0);
		if (rc < 0) {
			printf(" tima_rpcd_recv error");
		}

		if (strstr(recvbuf, "-gpsinfo")) {
			if (++count > 10) {
				printf("rc = %d, recv: %s\n", rc, recvbuf);
				count = 0;
			}
		} else {
			printf("rc = %d, recv: %s\n", rc, recvbuf);
		}
		tima_command_mock(fd, recvbuf);
	}

	tima_command_close(fd);

	return name;
}

static int test_mock(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2, pth_test1;

	char *client1 = "cmd";
	ret = pthread_create(&pth_test1, NULL, command_mock, (void*)client1);
	if (ret != 0)
		printf("create thread \'client1\' failed");


	char *client2 = "event";
	ret = pthread_create(&pth_test2, NULL, event_post, (void*)client2);
	if (ret != 0)
		printf("create thread \'client2\' failed");


	char *client3 = "reply";
	ret = pthread_create(&pth_test3, NULL, command_reply_async, (void*)client3);
	if (ret != 0)
		printf("create thread \'client3\' failed");

	pthread_join(pth_test3, &thread_result);
	pthread_join(pth_test2, &thread_result);
	pthread_join(pth_test1, &thread_result);

	return 0;
}
#else
static int test_mock(int argc, char *argv[])
{
	return 0;
}
#endif // TIMA_PROTOCOL

int mock_main(int argc, char *argv[])
{
	char* type = "mock";

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
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		switch(o) {
			case 't':
				type = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s", type);
	test_mock(argc, argv);

	return 0;
}
/*
int main(int argc, char **argv)
{
	int rc = 0;
	if (argc < 2) {
		fprintf (stderr, "Usage: %s [-p]\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	if (strcmp(argv[1], "-p") == 0) {
		//event_post("POST", (char*)str2);
	} else {
		test_mock(argc, argv);
	}

	exit (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
*/
