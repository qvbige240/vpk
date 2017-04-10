/*
 * History:
 * ================================================================
 * 2017-04-10 qing.zou created
 *
 */
#ifndef VPK_IFILESYS_H
#define VPK_IFILESYS_H

#include "vpk_typedef.h"
#include "vpk_object.h"

TIMA_BEGIN_DELS

typedef struct vpk_filesys_t vpk_filesys_t;

typedef struct filesys_ops_s {
	vpk_filesys_t* (*init)(const char* argv0);
	const char* (*app_path)(void);
	int (*exists)(const char *path);
	int (*isdir)(const char* path);
	int (*remove)(const char *path);
	int (*mkdir)(const char *path);
	void (*enumerate)(const char *dir, vpk_enumerate_callback callback, void *ctx);
	const char* (*drive_path)(vpk_disk_type_t t);
	int (*drive_size_all)(vpk_disk_type_t t);
	int (*drive_size_left)(vpk_disk_type_t t);
	int (*drive_connect)();
	int (*drive_disconnect)();
	int (*drive_is_connected)();
} filesys_ops;


#define extends_filesys()		\
	extends_object();			\
	const filesys_ops* f_ops

struct vpk_filesys_t
{
	extends_filesys();
};


TIMA_END_DELS

#endif //VPK_IFILESYS_H
