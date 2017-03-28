/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_OBJECT_H
#define VPK_OBJECT_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

typedef int (*vpk_destruct_func)(void *ctx);

#define extends_object()		\
	vpk_destruct_func destruct

TIMA_END_DELS

#endif //VPK_OBJECT_H
