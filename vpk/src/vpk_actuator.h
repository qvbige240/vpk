/**
 * History:
 * ================================================================
 * 2018-01-03 qing.zou created
 *
 */
#ifndef VPK_ACTUATOR_H
#define VPK_ACTUATOR_H

#include "vpk_typedef.h"
#include "vpk_session.h"

TIMA_BEGIN_DELS

#define extends_actuator()		\
	extends_session();			\
	void*	 data_buff;         \
	uint32_t total_size

struct vpk_actuator_t;
typedef struct vpk_actuator_t vpk_actuator_t;

struct vpk_actuator_t
{
	extends_actuator();
};

int register_actuator_device(const char* standard, const session_ops* sops);

/**
 * Create a actuator object.
 *
 * @param name		 The char device alias name.
 *
 * @return The actuator object.
 */
vpk_actuator_t* vpk_actuator_create(void *name);

/**
 * Open a actuator object.
 *
 * @param thiz	The target actuator device.
 *
 * @return 0 success, -1 fail.
 */
int vpk_actuator_open(vpk_actuator_t* thiz);

/**
 * Write data to a actuator device.
 *
 * @param thiz		The actuator device object.
 * @param buf		The buffer, write the target data to actuator.
 * @param nbytes	The data size, need write to the actuator.
 * @param timout	value 0.
 * @param recv_buff The buffer to receive the return value.
 * @param size_buff The total size of recv_buff, which is preset.
 *
 * @return .
 */
int vpk_actuator_write(vpk_actuator_t* thiz, void* buf, ssize_t nbytes, 
							  void* recv_buff, ssize_t size_buff, int timout);

/**
 * Close a actuator object.
 *
 * @param thiz	The target actuator device.
 *
 * @return 0 success, -1 fail.
 */
int vpk_actuator_close(vpk_actuator_t* thiz);

/**
 * Destroy a actuator object.
 *
 * @param thiz	The target actuator device.
 *
 * @return 0 success, -1 fail.
 */
int vpk_actuator_destroy(vpk_actuator_t* thiz);

TIMA_END_DELS

#endif //VPK_ACTUATOR_H
