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

#include "timapi/tima_eventq.h"
#include "timapi/tima_command.h"

#include "vpk.h"

#define BUFFER_SIZE			EVENT_QUEUE_MSG_LEN_MAX

const char *str1 = "{\"event\":\"3001\",\"msg\":null}";
const char *str2 = "{\"event\":\"2001\",\"msg\":{\"id\":0,\"key\":\"2001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"/xxx/01.mp4\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"/xxx/02.mp4\\\"}]\"}}";
const char *rec_str = "{\"event\":\"5001\",\"msg\":{\"id\":0,\"key\":\"5001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"2017_a.TS\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"2017_b.TS\\\"}]\"}}";

//const char *str2 = "{\"event\":\"5001\",\"msg\":{\"value\":\"{\\\"type\\\":0}\"}}";
//const char *str = "{\"event\":\"5001\",\"msg\":{\"id\":0,\"key\":\"5001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"/xxx/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/01.mp4\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"/xxx/02.mp4\\\"}]\"}}";

static void *event_post(void *name)
{
	int ret = 0;
	//char buffer[BUFFER_SIZE] = {0};

	//int key = tima_mqueue_open(EVENT_MQ_PATH, 'a');
	int key = tima_mqueue_open();
	
	//printf("str len = %d", (int)strlen(str));
	//strncpy(buffer, str, BUFFER_SIZE);

	char ch = '0';
	while (1) {
		printf("Enter message to reply: \n");
		scanf("%c", &ch);
		//ret = tima_mqueue_post(key, (void*)str2);
		ret = tima_mqueue_post(key, (void*)rec_str);
		printf("[%s]post: %s, ret = %d\n", (char*)name, str2, ret);
		sleep(1);
	}

	return name;
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
			const char *str = "{\"event\":\"5002\",\"msg\":{\"id\":0,\"key\":\"5002\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":0,\\\"file\\\":\\\"P70625-161747.jpg\\\"},{\\\"camera\\\":1,\\\"type\\\":0,\\\"file\\\":\\\"02.JPG\\\"}]\"}}";
			rc = tima_command_send(fd, str, strlen(str), 0);
			printf("rc = %d, send: %s\n", rc, str);
			return 0;
		}
		// VIDEO
		p = strstr(buf, "-movierec");
		if (p != NULL)
		{
			//const char *str = "{\"event\":\"5001\",\"msg\":{\"id\":0,\"key\":\"5001\",\"rcode\":0,\"value\":\"[{\\\"camera\\\":0,\\\"type\\\":1,\\\"file\\\":\\\"2017_a.TS\\\"},{\\\"camera\\\":1,\\\"type\\\":1,\\\"file\\\":\\\"2017_b.TS\\\"}]\"}}";
			const char *str = "0";
			rc = tima_command_send(fd, str, strlen(str), 0);
			printf("rc = %d, send: %s\n", rc, str);
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

	}
	return ret;
}

static void *command_mock(void *name)
{
	int rc;
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
		printf("rc = %d, recv: %s\n", rc, recvbuf);
		tima_command_mock(fd, recvbuf);
	}

	tima_command_close(fd);

	return name;
}

static int test_mock(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2;

	char *client1 = "cmd";
	ret = pthread_create(&pth_test3, NULL, command_mock, (void*)client1);
	if (ret != 0)
		printf("create thread \'client1\' failed");

	sleep(1);

	char *client2 = "event";
	ret = pthread_create(&pth_test2, NULL, event_post, (void*)client2);
	if (ret != 0)
		printf("create thread \'client2\' failed");

	pthread_join(pth_test3, &thread_result);
	pthread_join(pth_test2, &thread_result);

	return 0;
}

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
