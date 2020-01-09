/**
 * History:
 * ================================================================
 * 2019-12-24 qing.zou created
 *
 */

#ifndef VPK_BARRIER_H
#define VPK_BARRIER_H

#include "vpk_typedef.h"

VPK_BEGIN_DELS

struct vpk_barrier;
typedef struct vpk_barrier vpk_barrier_t;

vpk_barrier_t *vpk_barrier_create(void);
int vpk_barrier_init(vpk_barrier_t *barrier, int nthread);
int vpk_barrier_wait(vpk_barrier_t *barrier);
int vpk_barrier_destroy(vpk_barrier_t *barrier);

VPK_END_DELS

#endif // VPK_BARRIER_H
