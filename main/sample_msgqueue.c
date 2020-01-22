/**
 * History:
 * ================================================================
 * 2019-19-24 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>

#include "vpk.h"

extern void *vpk_test2(void* arg);
extern void *vpk_test3(void* arg);

#define HEXSTR_PARSE(ptr)	\
	((vpk_hex_to_int(*(ptr)) << 12) | (vpk_hex_to_int(*((ptr)+1)) << 8) \
	| (vpk_hex_to_int(*((ptr)+2)) << 4) | vpk_hex_to_int(*((ptr)+3)))

#ifdef USE_ZLOG
#include "vpk.h"
#define SAMPLE_ZLOG_FILE_PATH "."
#define SAMPLE_ZLOG_CONF_FILE "./zlog.conf"
int sample_zlog_init(int procname)
{
    int rc;

    if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH))
    {
        int ret = 0;
        char tmp[256] = {0};
        vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
        printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
        ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
        printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
    }

    rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
    if (rc)
    {
        printf("zlog init failed\n");
        return -1;
    }

    LOG_D("hello, zlog");

    return 0;
}
#endif

int keycode = 0;

/* post: ./msgqueue 1 */
int main(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	vpk_logging_level_set("DEBUG");

#ifdef USE_ZLOG
    sample_zlog_init(0);
#endif // USE_ZLOG

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	while ((o = getopt_long(argc, argv, "hk:l", long_options, NULL)) >= 0) {
		switch(o) {
			case 'k':
				keycode = HEXSTR_PARSE(optarg);
				LOG_D("keycode: %x", keycode);
				break;
			default:
				break;
		}
	}

    if (argc > 1)
    {
        ret = pthread_create(&pth_test3, NULL, vpk_test3, (void *)NULL);
        if (ret != 0)
            LOG_E("create thread \'vpk_test3\' failed");
        pthread_join(pth_test3, &thread_result);
    }
    else
    {
        ret = pthread_create(&pth_test2, NULL, vpk_test2, (void *)NULL);
        if (ret != 0)
            LOG_E("create thread \'vpk_test2\' failed");

        pthread_join(pth_test2, &thread_result);
    }

    return 0;
}

int test_msg_post(const char* name)
{
	int ret = -1;
	vpk_msg_t msg = {0};
	vpk_mqueue_t* mqueue = NULL;
	return_val_if_fail(name != NULL, -1);

	mqueue = vpk_mqueue_open(name, 'b');
	return_val_if_fail(mqueue != NULL, -1);

	LOG_D("send queue start");
    char tmp[256];
    sprintf(tmp, "{\"event\": \"%x\", \"msg\":null}", keycode);
    // msg.data = "{\"event\": \"6002\", \"msg\":null}";
    msg.data = tmp;
	while (1)
	{
		ret = vpk_mqueue_post(mqueue, &msg);
		LOG_I("[%s] ret = %d, post \n", name, ret);
        if (ret < 0)
            break;
		sleep(1);
	}

	vpk_mqueue_close(mqueue);

	return ret;
}

int test_msg_recv(const char* name)
{
	int ret = -1;
	vpk_msg_t msg = {0};
	vpk_mqueue_t* mqueue = NULL;
	return_val_if_fail(name != NULL, -1);

	mqueue = vpk_mqueue_open(name, 'b');
	return_val_if_fail(mqueue != NULL, -1);

	LOG_D("recv queue start");
	while (1)
	{
		memset(&msg, 0x00, sizeof(vpk_msg_t));
		ret = vpk_mqueue_recv(mqueue, &msg);
		LOG_I("[%s] ret = %d, recv %s\n", name, ret, msg.data);
        if (ret < 0)
            break;
		sleep(1);
	}

	vpk_mqueue_delete(mqueue);

	return ret;
}

void *vpk_test3(void* arg)
{
	sleep(1);
	LOG_D("[post]start test3 thread!");
	while(1)
	{
		LOG_D("test3 thread run.\n");
		test_msg_post("/tmp/tima/ipc2");
		sleep(1);
	}
}

void *vpk_test2(void* arg)
{
	sleep(1);
	LOG_D("[recv]start test2 thread!");
	while(1)
	{
		LOG_D("test2 thread run.");
		test_msg_recv("/tmp/tima/ipc2");
		sleep(1);
	}
}
