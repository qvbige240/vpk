/**
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#ifndef VPK_LIBC_H
#define VPK_LIBC_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

/**
 *	system call function with time out.
 */
VPKAPI int vpk_system_ex(const char *cmd, unsigned int timout);

/**
 *	system function, exec a command and return a buffer.
 */
VPKAPI int vpk_system_exval(char* cmd, char* buf, int size);

TIMA_END_DELS

#endif/*VPK_LIBC_H*/
