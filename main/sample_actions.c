/**
 * History:
 * ================================================================
 * 2017-05-28 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include "vpk.h"


static int action_callback_test(void* ctx, void* data)
{
	LOG_D("recv str: %s", (char*)data);
	return 0;
}

static int actions_test()
{
	int ret = 0;
	char type[64] = {0};
	char param[256] = {0};
	//char buffer[256] = {0};
	char recvbuf[256] = {0};
	VpkAction* actions = vpk_action_create(VPK_ACTION_DEVINFO, NULL, recvbuf, 256);
	while (1) {
		//memset(buffer, 0, sizeof(buffer));
		memset(type, 0, sizeof(type));
		memset(param, 0, sizeof(param));
		printf("Enter message to send(type+param: '4 121212 '): \n");
		scanf("%s %s", type, param);
		LOG_D("type = %s, param = %s", type, param);
		//LOG_D("buffer = %s", buffer);
		//sscanf(buffer, "%[^ ] %s", type, param);
		//char *str = "22 abcdef";
		//sscanf(str, "%[^ ] %s", type, param);
		//LOG_D("type = %s, param = %s", type, param);
		int i = atoi(type);
		vpk_action_param_set(actions, i, param);
		vpk_action_exec(actions, action_callback_test, NULL);

	}
	return ret;
}

int actions_main(int argc, char *argv[])
{
 	//int ret = 0;
 
 	//vpk_system_init(argc, argv);
 	//vpk_logging_level_set("DEBUG");

	char* type = "actions";
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

	if (strcasecmp(type, "tima") == 0) {
		actions_test();
	} else if (strcasecmp(type, "ucustom") == 0) {

	} else {
		actions_test();
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);		//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
