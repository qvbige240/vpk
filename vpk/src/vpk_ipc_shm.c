/**
 * History:
 * ================================================================
 * 2013-3-28 qing.zou created
 *
 */
#include <errno.h>

#include "vpk_ipc.h"
#include "vpk_ipc_shm.h"


int vpk_shmget(key_t key, int size, int shmflg)
{
	int shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);		/** create share memory **/
	if (shm_id < 0) {
		printf("share memory already exists and get!\n");
		shm_id = shmget(key, size, IPC_EXCL | 0666);
	}

	if (shm_id < 0) {
		printf("shmget failed! %s size:%d key:0x%08x\n", strerror(errno), size, key);
		return -1;
	}

	printf("create shared-memory id: %d size:%d key:0x%08x\n", shm_id, size, key);
	return shm_id;
}

void *vpk_shmat(int shmid, const void* shmaddr, int shmflg)
{
	void* shm_addr = shmat(shmid, NULL, 0);

	if (shm_addr == (void*)-1)
	{
		printf("[%d]shmat attached error(%s)!\n", shmid, strerror(errno));	
		return (void*)-1;
	}

	printf("[%d]attached shared-memory: %p\n", shmid, shm_addr);
	return shm_addr;
}

int vpk_shmdt(const void* shmaddr)
{
	int detach = shmdt(shmaddr);
	if (detach < 0) {
		printf("[%p]detaches shared-memory error!\n", shmaddr);
		return -1;
	}

	printf("detaches shared-memory.\n");
	return 0;
}

int vpk_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	int ret = shmctl(shmid, cmd, buf);

	if (ret == -1)
	{
		printf("[%d]shmctl command[%d] error!\n", shmid, cmd);
		return -1;
	}

	printf("command[%d] shared-memory!\n", cmd);
	return ret;
}
