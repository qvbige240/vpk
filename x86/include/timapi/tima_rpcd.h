/**
 * History:
 * ================================================================
 * 2017-08-30 qing.zou created
 *
 */
#include "tima_typedef.h"

#ifndef TIMA_RPCD_H
#define TIMA_RPCD_H

TIMA_EXTERN_BEGIN

/**
 * Initialize the system.
 * 
 */
//TIMAPI int vpk_system_init(int argc, char* argv[]);

TIMAPI int tima_rpcd_open(void);

TIMAPI int tima_rpcd_recv(int fd, void *buf, size_t len, int flags);

TIMAPI int tima_rpcd_send(int fd, const void *buf, size_t len, int flags);

TIMAPI int tima_rpcd_close(int fd);

TIMA_EXTERN_END

#endif // TIMA_RPCD_H
