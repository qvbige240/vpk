#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "vpk.h"

struct csq_data
{
	//	char data[MAX_CSQ_DATA];
	int data;
	int  len;
};

vpk_tqueue *my_queue = NULL;

void *vpk_mycsq_read_thread(void *arg);
void *vpk_mycsq_write_thread(void *arg);

int vpk_tqueue_test(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_csq_read, pth_csq_write;

	my_queue = vpk_tqueue_create(sizeof(struct csq_data), 10);

	ret = pthread_create(&pth_csq_read, NULL, vpk_mycsq_read_thread, (void*)NULL);
	 	if (ret != 0)
	 		LOG_E("create thread \'vpk_mycsq_read_thread\' failed");

	ret = pthread_create(&pth_csq_write, NULL, vpk_mycsq_write_thread, (void*)NULL);
	 	if (ret != 0)
	 		LOG_E("create thread \'vpk_mycsq_write_thread\' failed");

	pthread_join(pth_csq_read, &thread_result);
	pthread_join(pth_csq_write, &thread_result);

	return 0;
}

void *vpk_mycsq_read_thread(void *arg)
{
	struct csq_data csq = {0};

	LOG_I("vpk_mycsq_read_thread start\n");
	while (1)
	{
		while (vpk_tqueue_read(my_queue, (void *)&csq, 2) == 0)
		{
			//printf("read csq: %d\n", csq.data);
			LOG_I("read csq: %d\n", csq.data);
			sleep(1);
		}
		sleep(2);
	}	
}

void *vpk_mycsq_write_thread(void *arg)
{
	int ret = 0;
	struct csq_data csq = {0};

	csq.data = 1234;
	csq.len = 4;

	LOG_I("vpk_mycsq_write_thread start\n");
	while (1)
	{
		ret = vpk_tqueue_write(my_queue, (void *)&csq);
		LOG_D("write csq %d, ret = %d", csq.data, ret);

		csq.data++;
		sleep(1);
	}
}
