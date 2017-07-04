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
#include <time.h>
#include <sys/time.h>

#include "vpk.h"

#if defined(_X86_)
#else
#endif

#include <errno.h>

void time_test()
{
	time_t timestamp;
	struct tm *locals;
	//char timestr[] = "2018-11-01 11:30:00";

	timestamp = time(NULL);
	LOG_D("timestamp: %ld", timestamp);
	LOG_D("ctime: %s", ctime(&timestamp));
	locals = localtime(&timestamp);
	LOG_D("%d-%d-%d %02d:%02d:%02d", 
		locals->tm_year+1900, locals->tm_mon+1, locals->tm_mday, 
		locals->tm_hour, locals->tm_min, locals->tm_sec);

	timestamp = mktime(locals);
	LOG_D("timestamp: %ld", timestamp);
}

#if 1
static void set_timer(int seconds, int mseconds)
{
	struct timeval temp;

	temp.tv_sec = seconds;
	temp.tv_usec = mseconds;

	select(0, NULL, NULL, NULL, &temp);
	printf("timer\n");

	return ;
}

#elif 0

#elif 0

#endif

int timer_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	//char* pathname = "./ipc";
	//int proj_id = 0x10;
	char* type = "timer";
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

	time_test();

	gettimeofday(&prev, 0);

	set_timer(5, 10);

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);
	//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
