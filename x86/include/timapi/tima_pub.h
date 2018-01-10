/**
 * History:
 * ================================================================
 * 2017-09-07 qing.zou created
 *
 */
#include "tima_typedef.h"

#ifndef TIMA_PUB_H
#define TIMA_PUB_H

TIMA_EXTERN_BEGIN

/**
 * Initialize the system.
 * 
 */
//TIMAPI int vpk_system_init(int argc, char* argv[]);


TIMAPI int tima_pub_open(void);

TIMAPI int tima_pub_send(int fd, const void *buf, size_t len, int flags);

//TIMAPI int tima_pub_recv(int fd, void *buf, size_t len, int flags);

TIMAPI int tima_pub_close(int fd);

TIMA_EXTERN_END

#endif // TIMA_PUB_H
