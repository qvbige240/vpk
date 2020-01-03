/**
 * History:
 * ================================================================
 * 2019-12-24 qing.zou created
 *
 */

#include <pthread.h>

#include "vpk_typedef.h"
#include "vpk_barrier.h"

struct vpk_barrier
{
    pthread_barrier_t   b;
};

vpk_barrier_t *vpk_barrier_create()
{
    vpk_barrier_t *thiz = calloc(1, sizeof(vpk_barrier_t));
    return thiz;
}
int vpk_barrier_init(vpk_barrier_t *barrier, int nthread)
{
    return pthread_barrier_init(&barrier->b, NULL, nthread);
}
int vpk_barrier_wait(vpk_barrier_t *barrier)
{
    return pthread_barrier_wait(&barrier->b);
}
int vpk_barrier_destroy(vpk_barrier_t *barrier)
{
    int ret = 0;
    if (barrier)
    {
        ret = pthread_barrier_destroy(&barrier->b);
        free(barrier);
    }

    return ret;
}
