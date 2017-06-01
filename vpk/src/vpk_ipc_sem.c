/**
 * History:
 * ================================================================
 * 2017-5-28 qing.zou created
 *
 */
#include <errno.h>

#include "vpk_ipc.h"
#include "vpk_ipc_sem.h"

int vpk_semget(key_t key, int nsems, int semflg)
{
	int semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);		/** get a semaphore **/
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

int vpk_semop(int semid, struct sembuf *sops, unsigned nsops)
{
	int ret = semop(semid, sops, nsops);
	if (ret < 0) printf("semop failed, %s\n", strerror(errno));
	return ret;
}

int vpk_sem_p(int semid)
{
	struct sembuf PSEM = {0, -1, SEM_UNDO};
	int ret = semop(semid, &PSEM, 1);
	if (ret == -1) {
		printf("error: P semaphore (%d) failed, %s\n", semid, strerror(errno));
	}
	return ret;
}

int vpk_sem_v(int semid)
{
	struct sembuf VSEM = {0, 1, SEM_UNDO};
	int ret = semop(semid, &VSEM, 1);
	if (ret == -1) {
		printf("error: V semaphore (%d) failed, %s\n", semid, strerror(errno));
	}
	return ret;
}
