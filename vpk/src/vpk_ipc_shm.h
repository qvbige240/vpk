/**
 * History:
 * ================================================================
 * 2013-3-28 qing.zou created, shared-memory
 *
 */

#ifndef _VPK_SHARE_MEM_H_
#define _VPK_SHARE_MEM_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

int vpk_shmget(key_t key, size_t size, int shmflg);
void *vpk_shmat(int shmid, const void* shmaddr, int shmflg);
int vpk_shmdt(const void* shmaddr);
int vpk_shmctl(int shmid, int cmd, struct shmid_ds *buf);

TIMA_END_DELS

#endif /*_VPK_SHARE_MEM_H_*/
