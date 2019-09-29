/**
 * History:
 * ================================================================
 * 2019-09-25 qing.zou created
 *
 */

#ifndef VPK_DARRAY_LIST_H
#define VPK_DARRAY_LIST_H

#include "vpk_typedef.h"
#include "vpk_darray.h"
#include "vpk_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct da_object_s
{
    list_t              node;

    size_t              slot;

    char                priv[ZERO_LEN_ARRAY];
} da_object_t;

typedef struct vpk_dalist_s
{
    vpk_darray_t        *array;
    list_t              head;

    void                *ctx;
    VpkDataDestroy      destroy;
} vpk_dalist_t;

vpk_dalist_t *vpk_dalist_create(size_t size, VpkDataDestroy destroy, void *ctx);
int vpk_dalist_add(vpk_dalist_t *thiz, size_t *out_slot, da_object_t *data);
int vpk_dalist_delete(vpk_dalist_t *thiz, size_t slot);
int vpk_dalist_get_by_index(vpk_dalist_t *thiz, size_t slot, da_object_t **data);
//int vpk_dalist_set_by_index(vpk_dalist_t *thiz, size_t slot, da_object_t *data);
int vpk_dalist_modify_by_index(vpk_dalist_t *thiz, size_t slot, VpkDataVisit modify, void *args);
list_t *vpk_dalist_clone_list(vpk_dalist_t *thiz, size_t el_size);
void vpk_dalist_destroy(vpk_dalist_t *thiz);

int vpk_dalist_foreach(vpk_dalist_t *thiz, VpkDataVisit visit, void *ctx);
size_t vpk_dalist_count(vpk_dalist_t *thiz);

#ifdef __cplusplus
}
#endif

#endif // VPK_DARRAY_LIST_H
