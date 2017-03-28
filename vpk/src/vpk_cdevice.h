/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_CDEVICE_H
#define VPK_CDEVICE_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

typedef struct cdevice_item_t
{
	char standard[12];
	const void* ops;
}cdevice_item_t;

#define	MAX_STANDARD_NUM	16

typedef struct cdevice_register_t
{
	size_t count;
	cdevice_item_t items[MAX_STANDARD_NUM];
}cdevice_register_t;

/**
 * Register a char device/protocol/standard to device register container.
 *
 * @param thiz		The Register container object.
 * @param standard	The target device which will be registered to container.
 * @param ops		The operations function of target device.
 * @param name		The container type.
 *
 */
int vpk_register_cdevice(cdevice_register_t* thiz, const char* standard, const void* ops, const char* name);

/**
 * Find a char device/protocol/standard from a device container.
 *
 * @param thiz		The Register container object.
 * @param standard	A standard/protocol/device which will be looked for in the container.
 */
const void* vpk_find_cdevice(cdevice_register_t* thiz, const char* standard);

TIMA_END_DELS

#endif //VPK_CDEVICE_H
