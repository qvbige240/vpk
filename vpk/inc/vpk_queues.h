/*
 * History:
 * ================================================================
 * 2015-5-29 qing.zou created
 *
 */
#ifndef VPK_QUEUES_H
#define VPK_QUEUES_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

struct vpk_tqueue;
typedef struct vpk_tqueue vpk_tqueue;

/**
 * Initializes a TQueue object.
 *
 * @param queue	 The TQueue object.
 * @param s_size The data size of 'structure'.
 * @param nr	 The max number of TQueue's element.
 *
 * @return NULL: error.
 */
VPKAPI vpk_tqueue* vpk_tqueue_create(int s_size, int nr);

/**
 * Write data to TQueue.
 *
 * @param queue	 The TQueue object.
 * @param data   The target data to write.
 *
 * @return -1: error; 0: successful.
 */
VPKAPI int vpk_tqueue_write(vpk_tqueue *queue, void *data);

/**
 * Read data from TQueue.
 *
 * @queue  The TQueue object.
 * @data   Read data to this pointer.
 * @timout > 0: 
 *		   = 0:
 *		   < 0:
 *
 * @return -1: error; 0: successful.
 */
VPKAPI int vpk_tqueue_read(vpk_tqueue *queue, void *data, int timout);

/**
 * Destroy the TQueue.
 *
 * @queue  The TQueue object.
 *
 */
VPKAPI void vpk_tqueue_destroy(vpk_tqueue *queue);

#if 0
/** message queue by use of 'system call' **/
struct vpk_mqueue;
typedef struct vpk_mqueue vpk_mqueue;

/**
 * Initializes a message queue object.
 *
 * @param fname	 The .
 * @param id	 The msg queue...
 * @param s_size The ....
 *
 * @return NULL: error.
 */
vpk_mqueue* vpk_mqueue_create(const char* fname, int id, int s_size);
int vpk_mqueue_send(vpk_mqueue *msg_queue, void *data);
int vpk_mqueue_recv(vpk_mqueue *msg_queue, void *data, int msgflg);
void vpk_mqueue_destroy(vpk_mqueue *msg_queue);
#endif

TIMA_END_DELS

#endif //VPK_QUEUES_H
