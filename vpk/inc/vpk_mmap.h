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

VpkMmap*	vpk_mmap_create(const char* filename, size_t offset, size_t size);
void*		vpk_mmap_data(VpkMmap* thiz);
size_t		vpk_mmap_length(VpkMmap* thiz);
void		vpk_mmap_destroy(VpkMmap* thiz);
int			vpk_mmap_exist(const char* filename);

TIMA_END_DELS

#endif /*VPK_MMAP_H*/
