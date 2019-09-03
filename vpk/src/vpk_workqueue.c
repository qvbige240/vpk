/**
* History:
* ================================================================
* 2019-07-05 qing.zou created
*
*/

#include "vpk_workqueue.h"

#define LL_ADD(item, list) { \
	item->prev = NULL; \
	item->next = list; \
	list = item; \
}

#define LL_REMOVE(item, list) { \
	if (item->prev != NULL) item->prev->next = item->next; \
	if (item->next != NULL) item->next->prev = item->prev; \
	if (list == item) list = item->next; \
	item->prev = item->next = NULL; \
}

static void *worker_function(void *ptr)
{
	vpk_job_t *job;
	vpk_worker_t *worker = (vpk_worker_t *)ptr;

	while (1) {
		/* Wait until we get notified. */
		pthread_mutex_lock(&worker->workqueue->jobs_mutex);
		while (worker->workqueue->waiting_jobs == NULL) {
			pthread_cond_wait(&worker->workqueue->jobs_cond, &worker->workqueue->jobs_mutex);
		}
		job = worker->workqueue->waiting_jobs;
		if (job != NULL) {
			LL_REMOVE(job, worker->workqueue->waiting_jobs);
		}
		pthread_mutex_unlock(&worker->workqueue->jobs_mutex);

		/* If we're supposed to terminate, break out of our continuous loop. */
		if (worker->terminate) break;

		/* If we didn't get a job, then there's nothing to do at this time. */
		if (job == NULL) continue;

		/* Execute the job. */
		job->func(job);
	}

	free(worker);
	pthread_exit(NULL);
}

int vpk_workqueue_init(vpk_workqueue_t *workqueue, int nrworkers)
{
	int i;
	vpk_worker_t *worker;
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;

	if (nrworkers < 1) nrworkers = 1;
	memset(workqueue, 0, sizeof(*workqueue));
	memcpy(&workqueue->jobs_mutex, &blank_mutex, sizeof(workqueue->jobs_mutex));
	memcpy(&workqueue->jobs_cond, &blank_cond, sizeof(workqueue->jobs_cond));

	for (i = 0; i < nrworkers; i++) {
		if ((worker = malloc(sizeof(vpk_worker_t))) == NULL) {
			printf("Failed to allocate workers %d\n", i);
			return 1;
		}
		memset(worker, 0, sizeof(*worker));
		worker->workqueue = workqueue;
		if (pthread_create(&worker->thread, NULL, worker_function, (void *)worker)) {
			printf("Failed to start worker threads\n");
			free(worker);
			return 1;
		}
		LL_ADD(worker, worker->workqueue->workers);
	}

	return 0;
}

vpk_workqueue_t *vpk_workqueue_create(int nrworkers)
{
	vpk_workqueue_t *wq = calloc(1, sizeof(vpk_workqueue_t));
	if (wq)
	{
		vpk_workqueue_init(wq, nrworkers);
	}

	return wq;
}

void vpk_workqueue_shutdown(vpk_workqueue_t *workqueue)
{
	vpk_worker_t *worker = NULL;

	/* Set all workers to terminate. */
	for (worker = workqueue->workers; worker != NULL; worker = worker->next) {
		worker->terminate = 1;
	}

	/* Remove all workers and jobs from the work queue.
	 * wake up all workers so that they will terminate. */
	pthread_mutex_lock(&workqueue->jobs_mutex);
	workqueue->workers = NULL;
	workqueue->waiting_jobs = NULL;
	pthread_cond_broadcast(&workqueue->jobs_cond);
	pthread_mutex_unlock(&workqueue->jobs_mutex);
}

void vpk_workqueue_add_job(vpk_workqueue_t *workqueue, vpk_job_t *job)
{
	/* Add the job to the job queue, and notify a worker. */
	pthread_mutex_lock(&workqueue->jobs_mutex);
	LL_ADD(job, workqueue->waiting_jobs);
	pthread_cond_signal(&workqueue->jobs_cond);
	pthread_mutex_unlock(&workqueue->jobs_mutex);
}
