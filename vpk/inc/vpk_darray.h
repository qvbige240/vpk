/**
 * History:
 * ================================================================
 * 2019-09-23 qing.zou created
 *
 */

#ifndef VPK_DARRAY_H
#define VPK_DARRAY_H

#include "vpk_typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef int (*VpkDataDestroy)(void *ctx, void *data);
typedef int (*VpkDataVisit)(void *ctx, void *data);
typedef int (*VpkDataCompare)(void *ctx, void *data);

typedef struct vpk_darray_s
{
    size_t              tails;

    size_t              size;
    size_t              capacity;

    size_t              cursor;
    void                **data;

    void                *ctx;
    VpkDataDestroy      destroy;
} vpk_darray_t;

vpk_darray_t *vpk_darray_create(size_t size, VpkDataDestroy destroy, void *ctx);
int vpk_darray_expand(vpk_darray_t *thiz, size_t need);
int vpk_darray_insert(vpk_darray_t *thiz, size_t index, void *data);
int vpk_darray_add(vpk_darray_t *thiz, size_t *out_index, void *data);
int vpk_darray_delete(vpk_darray_t *thiz, size_t index);
int vpk_darray_get_by_index(vpk_darray_t *thiz, size_t index, void **data);
int vpk_darray_set_by_index(vpk_darray_t *thiz, size_t index, void *data);
size_t vpk_darray_count(vpk_darray_t *thiz);
int vpk_darray_foreach(vpk_darray_t *thiz, VpkDataVisit visit, void *ctx);
int vpk_darray_find(vpk_darray_t *thiz, VpkDataCompare cmp, void *ctx);
void vpk_darray_destroy(vpk_darray_t *thiz);

#ifdef __cplusplus
}
#endif

#endif // VPK_DARRAY_H
