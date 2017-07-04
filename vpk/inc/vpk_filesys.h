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
 * @return 1 exists, others on failure.
 */
VPKAPI int vpk_exists(const char* path);

/**
 * Get the input file length.
 *
 * @param path	The input file.
 *
 * @return > 0 on file size, 0 on not file, -1 on failure.
 */
VPKAPI long vpk_file_length(const char* path);

/**
 * Whether the input path is directory.
 *
 * @param path	The input file or directory.
 *
 * @return 1 is dir, others on failure.
 */
VPKAPI int vpk_isdir(const char* path);

/**
 * create the input directory.
 *
 * @param path	The input file or directory.
 *
 * @return 0 on success, -1 on failure.
 */
VPKAPI int vpk_mkdir(const char* path);

/**
 * create multilevel input directory.
 *
 * @param path	The input directory.
 *
 * @return 0 on success, -1 on failure.
 */
VPKAPI int vpk_mkdir_mult(const char* path);

/**
 * create file.
 *
 * @param filename	The input file name.
 *
 * @return 0 on success, -1 on failure.
 */
VPKAPI int vpk_create_file(const char *filename);

/**
 * get directory of input file full path.
 *
 * @param full	The input file full path.
 * @param path	The output directory.
 *
 * @return 0 on success, -1 on failure.
 */
VPKAPI int vpk_pathname_get(const char* full, char* path);

/**
 * rename file.
 *
 * @param oldname	The input file which will be rename.
 * @param newname	The new name.
 *
 * @return 0 on success, -1 on failure.
 */
VPKAPI int vpk_rename(const char* oldname, const char* newname);

/**
 * remove the input path or file.
 *
 * @param path	The input file or directory.
 *
 * @return 0 on success, -1 on failure.
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
