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
 * Create an endpoint for communication.
 * 
 * @return A descriptor 
 */
TIMAPI int tima_command_open(void);

/**
 * Receive a message from a descriptor.
 *
 * @param fd		A communication descriptor.
 * @param buf		Points to a buffer where the message should be stored.
 * @param len		Specifies the length in bytes of the buffer pointed to by the 'buf' argument.
 * @param flags		Specifies the type of message reception. Values are formed by zero usually.
 * 
 * @return return the length of the message in bytes, -1 on failure. 
 */
TIMAPI int tima_command_recv(int fd, void *buf, size_t len, int flags);

TIMAPI int tima_command_send(int fd, const void *buf, size_t len, int flags);

TIMAPI int tima_command_close(int fd);

TIMA_EXTERN_END

#endif // TIMA_RPCD_H
