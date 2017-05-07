/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "vpk.h"

extern int vpk_tqueue_test(int argc, char *argv[]);

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

int main(int argc, char *argv[])
{
// 	void* thread_result;
// 	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	LOG_I("log level: %s\n", vpk_logging_level_get());
	vpk_logging_level_set("DEBUG");

	vpk_testbyte();

	vpk_wlan0_ifcheck();

	vpk_tqueue_test(argc, argv);

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
