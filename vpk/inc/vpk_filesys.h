/**
* History:
* ================================================================
* 2017-04-10 qing.zou created
*
*/
#ifndef VPK_FILESYS_H
#define VPK_FILESYS_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

typedef enum vpk_disk_type_t
{
	vpk_disk_type_program,
	vpk_disk_type_user,
	vpk_disk_type_sdcard,
} vpk_disk_type_t;


/**
 * Whether the input path exists.
 *
 * @param path	The input file or directory.
 *
 * @return 1 exists, others fail.
 */
VPKAPI int vpk_exists(const char* path);

/**
 * Whether the input path is directory.
 *
 * @param path	The input file or directory.
 *
 * @return 1 is dir, others fail.
 */
VPKAPI int vpk_isdir(const char* path);

/**
 * create the input directory.
 *
 * @param path	The input file or directory.
 *
 * @return 0 success, -1 fail.
 */
VPKAPI int vpk_mkdir(const char* path);

/**
 * remove the input path or file.
 *
 * @param path	The input file or directory.
 *
 * @return 0 success, -1 fail.
 */
VPKAPI int vpk_remove(const char* path);

/**
 * A callback function, traverse directory and do what.
 *
 * @param ctx			The callback context.
 * @param predir		The file prefix path.
 * @param fname			The file name.
 */
typedef void (*vpk_enumerate_callback)(void *ctx, const char *predir, const char *fname);

/**
 * traverse the input directory.
 *
 * @param folder		The input file or directory.
 * @param callback		The callback function.
 * @param ctx			The callback context.
 *
 */
VPKAPI void vpk_enumerate(const char* folder, vpk_enumerate_callback callback, void* ctx);

TIMA_END_DELS

#endif //VPK_FILESYS_H
