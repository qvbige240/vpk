/**
 * History:
 * ================================================================
 * 2017-5-28 qing.zou created, semaphore
 *
 */

#ifndef _VPK_SEMAPHORE_H_
#define _VPK_SEMAPHORE_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "vpk_typedef.h"

TIMA_BEGIN_DELS
/*
static INLINE int vpk_semget(key_t key, int nsems, int semflg)
{
	int semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);
	if (semid < 0) {
		printf("semaphore already exists, re-get!\n");
		semid = semget(key, nsems, IPC_EXCL | 0666);
	}

	if (semid < 0) {
		printf("semget failed! %s key:0x%08x\n", strerror(errno), key);
		return -1;
	}

	printf("get semaphore: %d key:0x%08x\n", semid, key);
	return semid;
}

static INLINE int vpk_semop(int semid, struct sembuf *sops, unsigned nsops)
{
	int ret = semop(semid, sops, nsops);
	if (ret < 0) printf("semop failed, %s\n", strerror(errno));
	return ret;
}

#define vpk_semctl	semctl
//int vpk_semctl(int semid, int semnum, int cmd, ...);
*/
TIMA_END_DELS

#endif /*_VPK_SEMAPHORE_H_*/
