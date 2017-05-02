/*
 * History:
 * ================================================================
 * 2015-5-29 qing.zou created
 *
 */
#ifndef VPK_TQUEUE_H
#define VPK_TQUEUE_H

#include "vpk_queues.h"

TIMA_BEGIN_DELS

// struct vpk_tqueue;
// typedef struct vpk_tqueue vpk_tqueue;

struct vpk_tqueue
{
	int data_size;
	int data_num;

	int curr_len;

	int pos_write;
	int pos_read;

	char *buffer;
	//int loss;

	pthread_mutex_t 	queue_mutex;
	sem_t				queue_sem;
};

TIMA_END_DELS

#endif //VPK_TQUEUE_H
