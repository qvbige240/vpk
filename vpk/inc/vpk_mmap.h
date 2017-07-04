/**
 * History:
 * ================================================================
 * 2017-04-16 qing.zou created
 *
 */
#ifndef VPK_MMAP_H
#define VPK_MMAP_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

struct _VpkMmap;
typedef struct _VpkMmap VpkMmap;

/**
 * Create a file map object.
 *
 * @param offset		The file position offset.
 * @param size			need size which read from file, -1 means total file
 *
 * @return The file map object which content length, data and so on.
 */
VpkMmap*	vpk_mmap_create(const char* filename, size_t offset, size_t size);
void*		vpk_mmap_data(VpkMmap* thiz);
size_t		vpk_mmap_length(VpkMmap* thiz);
void		vpk_mmap_destroy(VpkMmap* thiz);
int			vpk_mmap_exist(const char* filename);
int			vpk_mmap_save(const char* filename, void* data, size_t size);

TIMA_END_DELS

#endif /*VPK_MMAP_H*/
