/**
 * History:
 * ================================================================
 * 2019-07-05 qing.zou created
 *
 */
#ifndef VPK_WORKQUEUE_H
#define VPK_WORKQUEUE_H

#include <pthread.h>

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

struct vpk_job;

typedef struct vpk_worker {
	pthread_t				thread;
	int						terminate;
	struct vpk_workqueue	*workqueue;
	struct vpk_worker		*prev;
	struct vpk_worker		*next;
} vpk_worker_t;

typedef int (*job_function)(struct vpk_job *job);

typedef struct vpk_job {
	job_function		func;
	void				*user_data;
	struct vpk_job		*prev;
	struct vpk_job		*next;
} vpk_job_t;

typedef struct vpk_workqueue {
	vpk_worker_t		*workers;
	vpk_job_t			*waiting_jobs;
	pthread_mutex_t		jobs_mutex;
	pthread_cond_t		jobs_cond;
} vpk_workqueue_t;

int vpk_workqueue_init(vpk_workqueue_t *workqueue, int nrworkers);

vpk_workqueue_t *vpk_workqueue_create(int nrworkers);

void vpk_workqueue_shutdown(vpk_workqueue_t *workqueue);

void vpk_workqueue_add_job(vpk_workqueue_t *workqueue, vpk_job_t *job);


TIMA_END_DELS

#endif //VPK_WORKQUEUE_H
