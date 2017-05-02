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


int main(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	LOG_I("log level: %s\n", vpk_logging_level_get());
	vpk_logging_level_set("DEBUG");

	vpk_tqueue_test(argc, argv);

	return 0;
}

