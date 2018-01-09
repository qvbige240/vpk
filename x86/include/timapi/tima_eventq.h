/**
 * History:
 * ================================================================
 * 2017-09-01 qing.zou created
 *
 */
#include "tima_typedef.h"

#ifndef TIMA_EVENTQ_H
#define TIMA_EVENTQ_H

TIMA_EXTERN_BEGIN

/* Maximum number of bytes that can be written in a single message */
//#define EVENT_QUEUE_MSG_LEN_MAX		16
#define EVENT_QUEUE_MSG_LEN_MAX		512

#if 0
/**
 * Create and open a event message queue.
 *
 * @param fname		The identity of the file named by the given pathname(which must refer to an existing, accessible file)
 * @param id		The type of int, but still only 8 bits are used, such as 'a'
 *
 * @return The event message queue identifier associated with the value of the fname and id arguments.
 */
TIMAPI int tima_mqueue_open(char* fname, int id);
#else

/**
 * Create and open a event message queue.
 *
 * @return The event message queue identifier.
 */
TIMAPI int tima_mqueue_open(void);
#endif

/**
 * The current number of messages in queue is empty.
 *
 * @param qid		The message queue identifier.
 *
 * @return 0 is not empty, 1 is empty
 */
TIMAPI int tima_mqueue_is_empty(int qid);

/**
 * Send messages to a message queue.
 *
 * @param qid		The message queue identifier.
 * @param msg_value The messages to send.
 *
 * @return 0 on success, -1 on failure.
 */
TIMAPI int tima_mqueue_post(int qid, void* msg_value);

/**
 * Receive messages from message queue.
 *
 * @param qid		The message queue identifier.
 * @param msg_value The messages received.
 *
 * @return 0 on success, -1 on failure.
 */
TIMAPI int tima_mqueue_recv(int qid, void* msg_value);

TIMA_EXTERN_END

#endif // TIMA_EVENTQ_H
