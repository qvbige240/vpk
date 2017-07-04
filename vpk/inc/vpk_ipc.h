/**
 * History:
 * ================================================================
 * 2017-5-28 qing.zou created, inter-process communication
 *
 */

#ifndef _VPK_IPC_H_
#define _VPK_IPC_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

/************** semaphore operations **************/
/**
 *  Get a semaphore set identifier.
 *
 * @param key		Which the semaphore set identifier associated with.
 * @param nsems		The number of semaphore set
 * @param semflg	IPC_CREAT\IPC_EXCL..
 *
 * @return semid on success, -1 on failure.
 */
VPKAPI int vpk_semget(key_t key, int nsems, int semflg);

VPKAPI int vpk_semop(int semid, struct sembuf *sops, unsigned nsops);

VPKAPI int vpk_sem_p(int semid);

VPKAPI int vpk_sem_v(int semid);

#define vpk_semctl	semctl


/************** shared-memory operations **************/
/**
 *  Allocates a System V shared memory segment.
 *
 * @param key		Which the shared-memory segment associated with.
 * @param size		The shared-memory size
 * @param shmflg	IPC_CREAT\IPC_EXCL..
 *
 * @return shared memory identifier on success, -1 on failure.
 */
VPKAPI int vpk_shmget(key_t key, size_t size, int shmflg);

/**
 *  Attaches shared memory segment.
 *
 * @param shmid		The shared-memory segment identified.
 * @param shmaddr	NULL
 * @param shmflg	0
 *
 * @return shared memory addr on success, (void *)-1 on failure.
 */
VPKAPI void *vpk_shmat(int shmid, const void* shmaddr, int shmflg);

/**
 *  Detaches the shared memory segment located at addr.
 *
 * @param shmaddr	The address that the shared-memory segment detaches.
 *
 * @return 0 on success, -1 on failure, and errno is set to indicate the
 *  cause of the error.
 */
VPKAPI int vpk_shmdt(const void* shmaddr);

/**
 *  Shared memory control.
 *
 * @param shmid	 The shared-memory segment identified.
 * @param cmd	 Like IPC_RMID...
 * @param buf
 *
 * @return other on success, -1 on error, and errno is set appropriately.
 */
VPKAPI int vpk_shmctl(int shmid, int cmd, struct shmid_ds *buf);


TIMA_END_DELS

#endif /*_VPK_IPC_H_*/
