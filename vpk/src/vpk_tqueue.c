/**
 * History:
 * ================================================================
 * 2017-5-2 qing.zou created
 *
 */
#include <pthread.h>
#include <semaphore.h>

#include "vpk_queues.h"
#include "vpk_tqueue.h"
#include "vpk_logging.h"


vpk_tqueue* vpk_tqueue_create(int s_size, int nr)
{
	int ret = -1;
	vpk_tqueue* queue = TIMA_MALLOC(sizeof(vpk_tqueue));
	return_val_if_fail(queue != NULL && s_size > 0 && nr > 0, NULL);

	ret = pthread_mutex_init(&queue->queue_mutex, NULL);
	if (ret != 0)
		return NULL;

	ret = sem_init(&queue->queue_sem, 0, 0);
	if (ret != 0)
	{
		pthread_mutex_destroy(&queue->queue_mutex);
		return NULL;
	}

	queue->data_size	= s_size;
	queue->data_num		= nr;
	queue->buffer		= TIMA_MALLOC(s_size * nr);
	if (queue->buffer == NULL)
	{
		sem_destroy(&queue->queue_sem);
		pthread_mutex_destroy(&queue->queue_mutex);
		return NULL;
	}

	queue->curr_len		= 0;
	queue->pos_read		= 0;
	queue->pos_write	= 0;

	return queue;
}

void vpk_tqueue_destroy(vpk_tqueue *queue)
{
	if (queue)
	{
		if (queue->buffer)
			TIMA_FREE(queue->buffer);

		sem_destroy(&queue->queue_sem);
		pthread_mutex_destroy(&queue->queue_mutex);

		TIMA_FREE(queue);
	}
}

static INLINE int vpk_tqueue_lock(vpk_tqueue *queue)
{
	return_val_if_fail(queue != NULL, -1);	

	return pthread_mutex_lock(&queue->queue_mutex);
}

static INLINE int vpk_tqueue_unlock(vpk_tqueue *queue)
{
	return_val_if_fail(queue != NULL, -1);

	return pthread_mutex_unlock(&queue->queue_mutex);
}

static INLINE int tqueue_sem_post(vpk_tqueue *queue)
{
	int ret = -1;
	return_val_if_fail(queue != NULL, -1);

	ret = sem_post(&queue->queue_sem);
	if (ret != 0)
		LOG_E("tqueue sem post failed!");

	return ret;
}

static INLINE int tqueue_sem_wait(vpk_tqueue *queue)
{
	return_val_if_fail(queue != NULL, -1);

	return sem_wait(&queue->queue_sem);
}

static INLINE int tqueue_sem_trywait(vpk_tqueue *queue)
{
	return_val_if_fail(queue != NULL, -1);

	return sem_trywait(&queue->queue_sem);
}

static INLINE int tqueue_sem_timedwait(vpk_tqueue *queue, int timout)
{
	struct timespec ts;
	return_val_if_fail(queue != NULL, -1);
	
	ts.tv_sec  = time(NULL) + timout; //clock_gettime()
	ts.tv_nsec = 1;

	return sem_timedwait(&queue->queue_sem, &ts);
}

int vpk_tqueue_write(vpk_tqueue *queue, void *data)
{
	int ret = -1;
	long long int pos = 0;
	return_val_if_fail(queue != NULL && data != NULL, -1);

	vpk_tqueue_lock(queue);

	if (queue->curr_len >= queue->data_num)
	{
		vpk_tqueue_unlock(queue);
		LOG_E("tqueue is full!");
		return -1;
	}

	pos = queue->pos_write * queue->data_size;
	memcpy((void*)&queue->buffer[pos], data, queue->data_size);
	queue->curr_len++;
	queue->pos_write++;
	if (queue->pos_write >= queue->data_num)
	{
		queue->pos_write = 0;
	}

	ret = tqueue_sem_post(queue);

	vpk_tqueue_unlock(queue);

	return ret;
}

int vpk_tqueue_read(vpk_tqueue *queue, void *data, int timout)
{
	int ret = -1;
	long long int pos = 0;
	return_val_if_fail(queue != NULL && data != NULL, -1);

	if (timout > 0)
	{
		ret = tqueue_sem_timedwait(queue, timout);
	}
	else if (timout < 0)
	{
		ret = tqueue_sem_wait(queue);
	}
	else
	{
		ret = tqueue_sem_trywait(queue);
	}
	if (ret != 0)
	{
		return ret;
	}

	vpk_tqueue_lock(queue);
	if (queue->curr_len > 0)
	{
		pos = queue->pos_read * queue->data_size;
		memcpy(data, (void*)&queue->buffer[pos], queue->data_size);
		queue->curr_len--;
		queue->pos_read++;
		if (queue->pos_read >= queue->data_num)
		{
			queue->pos_read = 0;
		}
		ret = 0;
	}
	vpk_tqueue_unlock(queue);

	return ret;
}
