/**
 * History:
 * ================================================================
 * 2017-05-28 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include "vpk.h"

#if defined(_X86_)


#else


#endif

#define MAX_DATA_CHUNK_SIZE		(1024+512)

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>


#define MAX_SHM_SIZE	 2048

struct shared_use_st {
	int				written_by_you;
	unsigned int	watchdog;
	char			some_text[MAX_SHM_SIZE];
};

struct network_status {
    unsigned char dog;
};

struct tima_status {
    unsigned char dog;
};

struct app_status {
    struct network_status network;
    struct tima_status    tima;
};

typedef struct dev_status
{
    union {
        struct app_status appstatus;
        char              tp1[64];
    };
    int      upgrade;
} dev_status_t;

#if 0
int producer(key_t key)
{
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	char buffer[BUFSIZ];
	int shmid;

	shmid = shmget(key, sizeof(struct shared_use_st), 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1) 
	{
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
		shmid = shmget(key, sizeof(struct shared_use_st), IPC_EXCL);
		if (shmid == -1) {
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
			return -1;
		}
	}

	printf("shmid: %d\n", shmid);
	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) 
	{
		fprintf(stderr, "11shmat failed, %s \n", strerror(errno));
		return -1;
	}

	printf("Memory attached at %p\n", shared_memory);

	shared_stuff = (struct shared_use_st *)shared_memory;
	while(running) 
	{
		while(shared_stuff->written_by_you == 1) 
		{
			sleep(1);
			printf("waiting for client...\n");
		}
		printf("Enter some text: ");
		fgets(buffer, BUFSIZ, stdin);

		strncpy(shared_stuff->some_text, buffer, MAX_SHM_SIZE);
		shared_stuff->written_by_you = 1;

		if (strncmp(buffer, "end", 3) == 0) {
			running = 0;
		}
	}

	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		printf("shmdt failed\n");
		return -1;
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) 
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		printf("shmctl(IPC_RMID) failed\n");
		return -1;
	}
	return 0;
}

int consumer(key_t key)
{
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	int shmid;

	srand((unsigned int)getpid());
	shmid = shmget(key, sizeof(struct shared_use_st), 0666 | IPC_CREAT| IPC_EXCL);
	if (shmid == -1) {
		fprintf(stderr, "shmget re-get, %s \n", strerror(errno));
		shmid = shmget(key, sizeof(struct shared_use_st), IPC_EXCL);
		if (shmid == -1) {
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
			return -1;
		}
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	printf("shmid: %d\n", shmid);

	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed\n");
		return -1;
	}

	printf("Memory attached at %p\n", shared_memory);
	shared_stuff = (struct shared_use_st *)shared_memory;
	shared_stuff->written_by_you = 0;

	while(running) 
	{
		if (shared_stuff->written_by_you) 
		{
			printf("You wrote: %s", shared_stuff->some_text);

			sleep( rand() % 4 ); /* make the other process wait for us ! */
			shared_stuff->written_by_you = 0;

			if (strncmp(shared_stuff->some_text, "end", 3) == 0) {
				running = 0;
			}
		}
	}
	if (shmdt(shared_memory) == -1) 
	{
		fprintf(stderr, "shmdt failed\n");
		printf("shmdt failed\n");
		return -1;
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) 
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		printf("shmctl(IPC_RMID) failed\n");
		return -1;
	}
	return 0;
}
#elif 0
#define  SEM_KEY   6666

union semun {
	int setval;
	struct semid_ds *buf;
	unsigned short *array;
};

int producer(key_t key)
{
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	char buffer[BUFSIZ];
	int shmid;

	shmid = shmget(key, sizeof(struct shared_use_st), 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1) 
	{
		fprintf(stderr, "shmget failed, %s \n", strerror(errno));
		shmid = shmget(key, sizeof(struct shared_use_st), IPC_EXCL);
		if (shmid == -1) {
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
			return -1;
		}
	}

	printf("shmid: %d\n", shmid);
	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) 
	{
		fprintf(stderr, "11shmat failed, %s \n", strerror(errno));
		return -1;
	}
	printf("Memory attached at %p\n", shared_memory);

	int semid, ret;
	semid = semget(SEM_KEY, 2, IPC_CREAT | 0666); //取得信号量
	if (-1 == semid)
	{
		fprintf(stderr, "semget failed, %s \n", strerror(errno));
		return -1;
	}
	printf("semid = %d\n", semid);

	union semun sem_args;
	unsigned short array[2]={1, 1};
	sem_args.array = array;
	ret = semctl(semid, 1, SETALL, sem_args);//SETALL代表设置信号集中所有的信号量的值。1,代表2个，sem_args是具体初始化的值放在共用体中。
	if (-1 == ret)
	{
		fprintf(stderr, "semctl failed, %s \n", strerror(errno));
		return -1;
	}

	struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};
	struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};

	shared_stuff = (struct shared_use_st *)shared_memory;
	while(running) 
	{
		//while(shared_stuff->written_by_you == 1) 
		//{
		//	sleep(1);
		//	printf("waiting for client...\n");
		//}
		if (semop(semid, sem_opt_wait1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}

		printf("Enter some text: \n");
		//fgets(buffer, BUFSIZ, stdin);

		//strncpy(shared_stuff->some_text, buffer, MAX_SHM_SIZE);
		//shared_stuff->written_by_you = 1;

		if (semop(semid, sem_opt_wakeup1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}

		if (strncmp(buffer, "end", 3) == 0) {
			running = 0;
		}
		sleep(2);
	}

	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		printf("shmdt failed\n");
		return -1;
	}

	//if (shmctl(shmid, IPC_RMID, 0) == -1) 
	//{
	//	fprintf(stderr, "shmctl(IPC_RMID) failed\n");
	//	printf("shmctl(IPC_RMID) failed\n");
	//	return -1;
	//}
	return 0;
}

int consumer(key_t key)
{
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	int shmid;

	srand((unsigned int)getpid());
	shmid = shmget(key, sizeof(struct shared_use_st), 0666 | IPC_CREAT| IPC_EXCL);
	if (shmid == -1) {
		fprintf(stderr, "shmget re-get, %s \n", strerror(errno));
		shmid = shmget(key, sizeof(struct shared_use_st), IPC_EXCL);
		if (shmid == -1) {
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
			return -1;
		}
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	printf("shmid: %d\n", shmid);

	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed\n");
		return -1;
	}
	printf("Memory attached at %p\n", shared_memory);
	shared_stuff = (struct shared_use_st *)shared_memory;
	shared_stuff->written_by_you = 0;

	int semid, ret;
	semid = semget(SEM_KEY, 0, IPC_CREAT | 0666);	//取得信号量
	if (-1 == semid)
	{
		fprintf(stderr, "semget failed, %s \n", strerror(errno));
		return -1;
	}
	printf("semid = %d\n", semid);

	struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};
	struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};

	while(running) 
	{
		if (semop(semid, sem_opt_wait1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}
		//if (shared_stuff->written_by_you) 
		{
			//printf("You wrote: %s", shared_stuff->some_text);
			printf("consumer.\n");

			//sleep( rand() % 4 ); /* make the other process wait for us ! */
			//shared_stuff->written_by_you = 0;

			if (strncmp(shared_stuff->some_text, "end", 3) == 0) {
				running = 0;
			}
		}


		if (semop(semid, sem_opt_wakeup1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}
		sleep(1);
	}

	if(-1==semctl(semid,1,IPC_RMID,0))//删除信号量
	{
		fprintf(stderr, "semctl(IPC_RMID) failed\n");
		printf("semctl(IPC_RMID) failed\n");
		//return -1;
	}

	if (shmdt(shared_memory) == -1) 
	{
		fprintf(stderr, "shmdt failed\n");
		printf("shmdt failed\n");
		return -1;
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) 
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		printf("shmctl(IPC_RMID) failed\n");
		return -1;
	}
	return 0;
}
#elif 0   // dog 
#define  SEM_KEY   6666

union semun {
	int setval;
	struct semid_ds *buf;
	unsigned short *array;
};

int producer(key_t key)
{
	int running = 1;
	void *shared_memory = (void *)0;
    dev_status_t* g_status;
	//char buffer[BUFSIZ];
	int shmid;

	shmid = shmget(key, sizeof(dev_status_t), 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1) 
	{
		fprintf(stderr, "shmget failed, %s \n", strerror(errno));
		shmid = shmget(key, sizeof(dev_status_t), IPC_EXCL);
		if (shmid == -1) {
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
			return -1;
		}
	}

	printf("shmid: %d\n", shmid);
	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) 
	{
		fprintf(stderr, "11shmat failed, %s \n", strerror(errno));
		return -1;
	}
	printf("Memory attached at %p\n", shared_memory);

	int semid, ret;
	semid = semget(SEM_KEY, 2, IPC_CREAT | 0666); //取得信号量
	if (-1 == semid)
	{
		fprintf(stderr, "semget failed, %s \n", strerror(errno));
		return -1;
	}
	printf("semid = %d\n", semid);

	union semun sem_args;
	unsigned short array[2]={1, 1};
	sem_args.array = array;
	ret = semctl(semid, 1, SETALL, sem_args);//SETALL代表设置信号集中所有的信号量的值。1,代表2个，sem_args是具体初始化的值放在共用体中。
	if (-1 == ret)
	{
		fprintf(stderr, "semctl failed, %s \n", strerror(errno));
		return -1;
	}

	struct sembuf sem_opt_wait1 = {0, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup1 = {0, 1, SEM_UNDO};
	//struct sembuf sem_opt_wait2 = {1, -1, SEM_UNDO};
	//struct sembuf sem_opt_wakeup2 = {1, 1, SEM_UNDO};

	g_status = (dev_status_t *)shared_memory;
    g_status->appstatus.tima.dog = 0;
	while(running) 
	{
		if (semop(semid, &sem_opt_wait1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
            break;
		}

        g_status->appstatus.tima.dog++;
		printf("producer dog: %d\n", g_status->appstatus.tima.dog);

		if (semop(semid, &sem_opt_wakeup1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}

		if (g_status->appstatus.tima.dog == 20) {
			running = 0;
		}
		sleep(2);
	}

	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		printf("shmdt failed\n");
		return -1;
	}

	//if (shmctl(shmid, IPC_RMID, 0) == -1) 
	//{
	//	fprintf(stderr, "shmctl(IPC_RMID) failed\n");
	//	printf("shmctl(IPC_RMID) failed\n");
	//	return -1;
	//}
	return 0;
}

int consumer(key_t key)
{
	int running = 1;
	void *shared_memory = (void *)0;
    dev_status_t* g_status;
	int shmid;

	srand((unsigned int)getpid());
	shmid = shmget(key, sizeof(dev_status_t), 0666 | IPC_CREAT| IPC_EXCL);
	if (shmid == -1) {
		fprintf(stderr, "shmget re-get, %s \n", strerror(errno));
		shmid = shmget(key, sizeof(dev_status_t), IPC_EXCL);
		if (shmid == -1) {
			fprintf(stderr, "shmget failed, %s \n", strerror(errno));
			return -1;
		}
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	printf("shmid: %d\n", shmid);

	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed\n");
		return -1;
	}
	printf("Memory attached at %p\n", shared_memory);
	g_status = (dev_status_t *)shared_memory;

	int semid;
	semid = semget(SEM_KEY, 0, 0666 | IPC_CREAT| IPC_EXCL);	//取得信号量
	if (-1 == semid)
	{
		fprintf(stderr, "semget re-get, %s \n", strerror(errno));
		semid = semget(SEM_KEY, 0, 0666 | IPC_EXCL);	//取得信号量
		if (-1 == semid) {
			fprintf(stderr, "semget failed, %s \n", strerror(errno));
			return -1;
		}
	}
	printf("semid = %d\n", semid);

	struct sembuf sem_opt_wait1 = {0, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup1 = {0, 1, SEM_UNDO};
	//struct sembuf sem_opt_wait2 = {1, -1, SEM_UNDO};
	//struct sembuf sem_opt_wakeup2 = {1, 1, SEM_UNDO};

	while(running) 
	{
		if (semop(semid, &sem_opt_wait1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}
		{
			printf("consumer.\n");

            //g_status->appstatus.tima.dog++;
            printf("dog: %d\n", g_status->appstatus.tima.dog);

			if (g_status->appstatus.tima.dog == 20) {
				running = 0;
			}
		}

		if (semop(semid, &sem_opt_wakeup1, 1) == -1) {
			printf("semop failed, %s\n", strerror(errno));
		}
		sleep(1);
	}

	if(-1==semctl(semid,1,IPC_RMID,0))//删除信号量
	{
		fprintf(stderr, "semctl(IPC_RMID) failed\n");
		printf("semctl(IPC_RMID) failed\n");
		//return -1;
	}

	if (shmdt(shared_memory) == -1) 
	{
		fprintf(stderr, "shmdt failed\n");
		printf("shmdt failed\n");
		return -1;
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) 
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		printf("shmctl(IPC_RMID) failed\n");
		return -1;
	}
	return 0;
}
#elif 1
#include "vpk_ipc.h"
union semun {
	int setval;
	struct semid_ds *buf;
	unsigned short *array;
};

#define  shm_lock		vpk_sem_p
#define	 shm_unlock		vpk_sem_v

static int vpk_feed_dog(key_t keyshm, key_t keysem)
{
	dev_status_t* g_status;
	int shmid;

	shmid = vpk_shmget(keyshm, sizeof(dev_status_t), 0);	//
	if (shmid < 0)
		LOG_E("shmid error.");

	g_status = (dev_status_t*)vpk_shmat(shmid, NULL, 0);
	//memset(g_status, 0x00, sizeof(dev_status_t));

	int semid = vpk_semget(keysem, 1, 0);
	if (semid < 0)
		LOG_E("semid error.");

	//union semun sem_args;
	//unsigned short array[1]={1};
	//sem_args.array = array;
	//if (vpk_semctl(semid, 1, SETALL, sem_args) < 0)
	//	LOG_E("vpk_semctl error.");

	int dog = 0, ret = 0;
	while (1)
	{
		ret = shm_lock(semid);
		LOG_D("shm_lock ret=%d", ret);

		dog = g_status->appstatus.tima.dog = 20;

		shm_unlock(semid);
		LOG_D("feed dog %d", dog);
		sleep(5);
		//while(1) sleep(5);
	}

	vpk_shmdt((void*)g_status);

	return 0;
}

static int watch_dog(key_t keyshm, key_t keysem)
{
	dev_status_t* g_status;
	int shmid;

	shmid = vpk_shmget(keyshm, sizeof(dev_status_t), 0);	//
	if (shmid < 0)
		LOG_E("shmid error.");

	g_status = (dev_status_t*)vpk_shmat(shmid, NULL, 0);
	//memset(g_status, 0x00, sizeof(dev_status_t));

	int semid = vpk_semget(keysem, 1, 0);
	if (semid < 0)
		LOG_E("semid error.");

	union semun sem_args;
	unsigned short array[1]={1};
	sem_args.array = array;
	if (vpk_semctl(semid, 1, SETALL, sem_args) < 0)
		LOG_E("vpk_semctl error.");

	int dog = 0, ret = 0;
	while (1)
	{
		LOG_D("start sem p");
		ret = shm_lock(semid);
		LOG_D("start sem p after, ret=%d", ret);
		dog = g_status->appstatus.tima.dog--;
		shm_unlock(semid);
		if (dog == 0 || dog < 5)
			LOG_W("process no response or not running.");
			//break;
		LOG_D("watch dog = %d.", dog);
		sleep(1);
	}

	LOG_D("watch end");
	if(-1==semctl(semid,1,IPC_RMID,0))//删除信号量
	{
		fprintf(stderr, "semctl(IPC_RMID) failed\n");
		printf("semctl(IPC_RMID) failed\n");
		//return -1;
	}
	if (-1 == vpk_semctl(semid, 1, IPC_RMID, 0))
		LOG_E("semctl rm failed.");
	vpk_shmdt((void*)g_status);
	vpk_shmctl(shmid, IPC_RMID, 0);

	return 0;
}
#endif

int ipc_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	char* pathname = "./ipcs";
	int proj_id = 0x10;
	char* type = "consumer";
	//if (argc > 1)
	//{
	//	type = argv[1];
	//}

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 't':
				type = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s", type);

	key_t key = ftok(pathname, proj_id);
	key_t keysem = ftok(pathname, proj_id+5);

	if (strcasecmp(type, "consumer") == 0) {
		LOG_D("consumer start...");
		//consumer(key);
		watch_dog(key, keysem);
	}
	else {
		LOG_D("producer start...");
		//producer(key);
		vpk_feed_dog(key, keysem);
	}

	// ipcs
	// ipcrm -s 7864353

	return 0;
}
