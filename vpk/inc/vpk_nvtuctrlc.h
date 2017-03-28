/*
 * History:
 * ================================================================
 * 2017-03-28 qing.zou created
 *
 */
#ifndef VPK_NVTUCTRLC_H
#define VPK_NVTUCTRLC_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

struct vpk_nvtuctrl_t;
typedef struct vpk_nvtuctrl_t vpk_nvtuctrl_t;

/**
 * Create a nvtuctrl object.
 *
 * @param name		 The char device alias name.
 *
 * @return The nvtuctrl object.
 */
VPKAPI vpk_nvtuctrl_t* vpk_nvtuctrl_create(void *name);

/**
 * Open a nvtuctrl object.
 *
 * @param thiz	The target nvtuctrl device.
 *
 * @return 0 success, -1 fail.
 */
VPKAPI int vpk_nvtuctrl_open(vpk_nvtuctrl_t* thiz);

/**
 * Write data to a nvtuctrl device.
 *
 * @param thiz		The nvtuctrl device object.
 * @param buf		The buffer, write the target data to nvtuctrl.
 * @param nbytes	The data size, need write to the nvtuctrl.
 * @param timout	value 0.
 * @param recv_buff The buffer to receive the return value.
 * @param size_buff The total size of recv_buff, which is preset.
 *
 * @return .
 */
VPKAPI int vpk_nvtuctrl_write(vpk_nvtuctrl_t* thiz, void* buf, ssize_t nbytes, 
							  void* recv_buff, ssize_t size_buff, int timout);

/**
 * Close a nvtuctrl object.
 *
 * @param thiz	The target nvtuctrl device.
 *
 * @return 0 success, -1 fail.
 */
VPKAPI int vpk_nvtuctrl_close(vpk_nvtuctrl_t* thiz);

/**
 * Destroy a nvtuctrl object.
 *
 * @param thiz	The target nvtuctrl device.
 *
 * @return 0 success, -1 fail.
 */
VPKAPI int vpk_nvtuctrl_destroy(vpk_nvtuctrl_t* thiz);

TIMA_END_DELS

#endif //VPK_NVTUCTRLC_H
