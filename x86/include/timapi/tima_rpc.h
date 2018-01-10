/**
 * History:
 * ================================================================
 * 2017-08-30 qing.zou created
 *
 */
#include "tima_typedef.h"

#ifndef TIMA_RPC_H
#define TIMA_RPC_H

TIMA_EXTERN_BEGIN

/**
 * Initialize the system.
 * 
 */
//TIMAPI int vpk_system_init(int argc, char* argv[]);

TIMAPI int tima_rpc_open(void);
TIMAPI int tima_rpc_recv(int fd, void *buf, size_t len, int flags);
TIMAPI int tima_rpc_recv_alloc(int fd, void *buf, int flags);
TIMAPI int tima_rpc_send(int fd, const void *buf, size_t len, int flags);
TIMAPI int tima_rpc_freemsg(void *msg);
TIMAPI int tima_rpc_close(int fd);

TIMA_EXTERN_END

#endif // TIMA_RPC_H
