/**
 * History:
 * ================================================================
 * 2017-09-07 qing.zou created
 *
 */
#include "tima_typedef.h"

#ifndef TIMA_SUB_H
#define TIMA_SUB_H

TIMA_EXTERN_BEGIN

/**
 * Initialize the system.
 * 
 */

TIMAPI int tima_sub_open(void);

TIMAPI int tima_sub_recv(int fd, void *buf, size_t len, int flags);

//TIMAPI int tima_sub_send(int fd, const void *buf, size_t len, int flags);

TIMAPI int tima_sub_close(int fd);

TIMA_EXTERN_END

#endif // TIMA_SUB_H
