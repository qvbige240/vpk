/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>

#include "vpk.h"

extern int actions_main(int argc, char *argv[]);
extern int epoll_main(int argc, char *argv[]);
extern int socket_main(int argc, char *argv[]);
extern int minheap_main(int argc, char *argv[]);
extern int sqlite_main(int argc, char *argv[]);
extern int sort_main(int argc, char *argv[]);
extern int timer_main(int argc, char *argv[]);
extern int ipc_main(int argc, char *argv[]);
extern int filesys_main(int argc, char *argv[]);
extern int mqsend_main(int argc, char *argv[]);
extern int md5_main(int argc, char *argv[]);
extern int string_main(int argc, char *argv[]);
extern int download_main(int argc, char *argv[]);
extern int json_main(int argc, char *argv[]);
//extern int vpk_tqueue_test(int argc, char *argv[]);
extern int queue_main(int argc, char *argv[]);

// #define LOG_D(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_I(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_W(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_E(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)
// #define LOG_F(...)	do {printf(__VA_ARGS__); printf("\n");}while(0)

int vpk_wlan0_ifcheck(void);

int vpk_testbyte()
{
	LOG_I("short: %d, int %d, long %d, long int: %d, long long: %d, float: %d, double: %d\n",
		sizeof(short), sizeof(int), sizeof(long), sizeof(long int), sizeof(long long), sizeof(float), sizeof(double));

	time_t timestamp;
	int  time00 = 0;
	long time11 = 0;
	long long time22 = 0;
	long long time33 = 0;

	timestamp = time(NULL);
	time00 = timestamp*1000;
	time11 = (long)timestamp*1000;
	time22 = (long long)timestamp*1000;
	time33 = (long long)timestamp*1000L;

	LOG_I("time: %ld, time00: %ld, time11: %ld, time22: %lld, time33: %lld", 
		timestamp, time00, time11, time22, time33);

//pc out
// short: 2, int 4, long 8, long int: 8, long long: 8, float: 4, double: 8
// time: 1493886242, time00: 3532590288, time11: 1493886242000, time22: 1493886242000, time33: 1493886242000

//mips out
// short: 2, int 4, long 4, long int: 4, long long: 8, float: 4, double: 8
// time: 1451462994, time00: -235952048, time11: -235952048, time22: 1451462994000, time33: 145146299
// time: 1451462834, time00: -236112048, time11: -236112048, time22: 1451462834000, time33: 1451462834000
	return 0;
}

/*
static void help(FILE *f) {
	fprintf(f,
		"%s [options]\n"
		"    -h --help          Show this help\n"
		"    -D --daemonize     Daemonize after startup (implies -s)\n"
		"    -s --syslog        Write log messages to syslog(3) instead of STDERR\n"
		"    -k --kill          Kill a running daemon\n"
		"    -r --reload        Request a running daemon to reload static services\n"
		"    -c --check         Return 0 if a daemon is already running\n"
		"    -V --version       Show version\n"
		"    -f --file=FILE     Load the specified configuration file instead of\n"
		"                       "AVAHI_CONFIG_FILE"\n"
		"       --no-rlimits    Don't enforce resource limits\n"
		"       --no-drop-root  Don't drop privileges\n"
#ifdef ENABLE_CHROOT
		"       --no-chroot     Don't chroot()\n"
#endif
		"       --no-proc-title Don't modify process title\n"
		"       --debug         Increase verbosity\n",
		argv0);
}*/
#define SAMPLE_CONFIG_FILE	"./test"
static void help(FILE *f) {
	fprintf(f,
		"   [options]\n"
		"    -h --help          Show this help\n"
		"    -V --version       Show version\n"
		"    -d --sample        Target sample demo which will be running(mqsend, \n"
		"                       queue, download, md5, json, filesys, ipc, sort).\n"
		"                       timer, string, sqlite3, minheap, socket, epoll).\n"
		"    -t --type          Event type for keycode\n"
		"    -k --keycode       Key code value(4 bytes long, such as: 1001, 3001)\n"
		"    -u --url           Target remote url.\n"
		"    -f --file=FILE     Load the specified configuration file instead of\n"
		"                       "SAMPLE_CONFIG_FILE"\n"
		"    -r --reload        Request a running daemon to reload static services\n"
		"    -c --check         Return 0 if a daemon is already running\n"
		"    -l --syslog        Write log messages to syslog(3) instead of STDERR\n"
		"       --no-rlimits    Don't enforce resource limits\n"
		"       --no-proc-title Don't modify process title\n"
		"       --debug         Increase verbosity\n"
		);
}

typedef enum {
	SAMPLE_DEMO_STRING = 0,
	SAMPLE_DEMO_MQSEND = 1,
	SAMPLE_DEMO_MD5,
	SAMPLE_DEMO_DOWNLOAD,
	SAMPLE_DEMO_TQUEUE,
	SAMPLE_DEMO_JSON,
	SAMPLE_DEMO_FILESYS,
	SAMPLE_DEMO_IPC,
	SAMPLE_DEMO_TIMER,
	SAMPLE_DEMO_SORT,
	SAMPLE_DEMO_SQLITE,
	SAMPLE_DEMO_MINHEAP,
	SAMPLE_DEMO_SOCKET,
	SAMPLE_DEMO_EPOLL,
	SAMPLE_DEMO_ACTIONS,
} SampleDemo;

typedef struct _demo_exec
{
	int	 index;

	char* name;
} demo_exec;

static const demo_exec demo_tables[] = {
	{SAMPLE_DEMO_STRING,	"string"},
	{SAMPLE_DEMO_MQSEND,	"mqsend"},
	{SAMPLE_DEMO_MD5,		"md5"},
	{SAMPLE_DEMO_DOWNLOAD,	"download"},
	{SAMPLE_DEMO_TQUEUE,	"queue"},
	{SAMPLE_DEMO_JSON,		"json"},
	{SAMPLE_DEMO_FILESYS,	"filesys"},
	{SAMPLE_DEMO_IPC,		"ipc"},
	{SAMPLE_DEMO_TIMER,		"timer"},
	{SAMPLE_DEMO_SORT,		"sort"},
	{SAMPLE_DEMO_SQLITE,	"sqlite3"},
	{SAMPLE_DEMO_MINHEAP,	"minheap"},
	{SAMPLE_DEMO_SOCKET,	"socket"},
	{SAMPLE_DEMO_EPOLL,		"epoll"},
	{SAMPLE_DEMO_ACTIONS,	"actions"},
};

static int find_demo(const char* demo)
{
	int i = 0;
	int count = _countof(demo_tables);

	for (i = 0; i < count; i++)
	{
		if (strcmp(demo, demo_tables[i].name) == 0)
			return demo_tables[i].index;
	}

	return -1;
}

#ifdef USE_ZLOG
#define SAMPLE_ZLOG_FILE_PATH		"."
#define SAMPLE_ZLOG_CONF_FILE		"./zlog.conf"
int sample_zlog_init(int procname)
{
	int rc;
	//zlog_category_t *c;

	if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH)) {
		int ret = 0;
		char tmp[256] = {0};
		vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
		printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
		ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
		printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
	}

	//if (procname == PROCESS_APP)
	//	rc = dzlog_init(ZLOG_CONF_FILE, "app");
	//else if (procname == PROCESS_DAEMON)
	//	rc = dzlog_init(ZLOG_CONF_FILE, "timad");
	//else
	//	rc = dzlog_init(ZLOG_CONF_FILE, "app");

	rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
	if (rc)	{
		printf("zlog init failed\n");
		return -1;
	}

	//c = zlog_get_category("my_cat");
	//if (!c) {
	//	printf("get cat fail\n");
	//	zlog_fini();
	//	return -2;
	//}
	//zlog_info(c, "hello, zlog");

	LOG_D("hello, zlog");

	return 0;
}
#endif

int main(int argc, char *argv[])
{
	int o, hflag = 0, index;
	//char myargv[6][MAX_PATH_SIZE];

#ifdef USE_ZLOG
	sample_zlog_init(0);
#endif // USE_ZLOG

	vpk_system_init(argc, argv);
	LOG_I("log level: %s\n", vpk_logging_level_get());
	vpk_logging_level_set("DEBUG");

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

	LOG_I("optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		switch(o) {
			case 'V':
				LOG_I("default version");
				break;
			case 'h':
				hflag = 1;
				break;
			case 'd':
				index = find_demo(optarg);
				if (index < 0) {
					fprintf(stderr, "-d --sample arguments error\n");
					return 0;
				}
				break;
			case 't':
				//LOG_D("type: %s", optarg);
				break;
			case 'k':
				//LOG_D("keycode: %s", optarg);
				break;
			case 'f':
				break;
			case 'u':
				break;
			case 'l':
				break;
			default:
				return -1;
		}
	}

	if (hflag || argc == 1) {
		help(stdout);
		return 0;
	}

	LOG_I("optind = %d, argc = %d", optind, argc);
	if (optind < argc) {
		//LOG_W("optind = %d, argc = %d", optind, argc);
		fprintf(stderr, "Too many arguments\n");
		return -1;
	}

	if (index == SAMPLE_DEMO_STRING) {
		string_main(argc, argv);
	} else if (index == SAMPLE_DEMO_MQSEND) {
		mqsend_main(argc, argv);
	} else if (index == SAMPLE_DEMO_MD5) {
		md5_main(argc, argv);
	} else if (index == SAMPLE_DEMO_DOWNLOAD) {
		download_main(argc, argv);
	} else if (index == SAMPLE_DEMO_TQUEUE) {
		//vpk_tqueue_test(argc, argv);
		queue_main(argc, argv);
	} else if (index == SAMPLE_DEMO_JSON) {
		json_main(argc, argv);
	} else if (index == SAMPLE_DEMO_FILESYS) {
		filesys_main(argc, argv);
	} else if (index == SAMPLE_DEMO_IPC) {
		ipc_main(argc, argv);
	} else if (index == SAMPLE_DEMO_TIMER) {
		timer_main(argc, argv);
	} else if (index == SAMPLE_DEMO_SORT) {
		sort_main(argc, argv);
	} else if (index == SAMPLE_DEMO_SQLITE) {
		sqlite_main(argc, argv);
	} else if (index == SAMPLE_DEMO_MINHEAP) {
		minheap_main(argc, argv);
	} else if (index == SAMPLE_DEMO_SOCKET) {
		socket_main(argc, argv);
	} else if (index == SAMPLE_DEMO_EPOLL) {
		epoll_main(argc, argv);
	} else if (index == SAMPLE_DEMO_ACTIONS) {
		actions_main(argc, argv);
	} else {
		fprintf(stderr, "\n\n-d --sample without arguments error\n");
		vpk_testbyte();
		vpk_wlan0_ifcheck();
		string_main(argc, argv);
	}

#if 0
	vpk_testbyte();
	vpk_wlan0_ifcheck();
#elif 0
	mqsend_main(argc, argv);
#elif 0
	md5_main(argc, argv);
#elif 0
	string_main(argc, argv);
#elif 0
	download_main(argc, argv);
#elif 0
	json_main(argc, argv);
#elif 0
	vpk_tqueue_test(argc, argv);
#endif

	return 0;
}

#include <ctype.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

int vpk_dev_ifaddr_get(const char *dev_name, char *ip_addr)
{
	int sockfd, ret = -1;
	struct ifreq ifr;
	char tmp[16] = {0};
	return_val_if_fail(dev_name != NULL && ip_addr != NULL, -1);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		LOG_E("Create socket fd(sockfd) fail!");
		return -1;
	}

	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, 5);
	if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0)
	{
		char *addr = (char *)&ifr.ifr_ifru.ifru_addr.sa_data + sizeof(unsigned short int);
		//char *addr = (char *)&ifr.ifr_ifru.ifru_addr.sa_data;
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
		LOG_D("ip address: %s", tmp);
		strcpy(ip_addr, tmp);
		ret = 0;
	}
	else
	{
		LOG_E("ioctl error at get ifaddr!");
		ret = -1;
	}

	close(sockfd);
	return ret;
}

int vpk_wlan0_ifcheck(void)
{
	int ret = -1;
	char ipv4_buf[16] = {0};

	ret = vpk_dev_ifaddr_get("eth0", ipv4_buf);

	return ret;
}
