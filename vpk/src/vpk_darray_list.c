/**
 * History:
 * ================================================================
 * 2019-09-25 qing.zou created
 *
 */

#include "vpk_darray_list.h"

static int dalist_destroy_data(void *ctx, void *data)
{
    vpk_dalist_t *thiz = ctx;
    da_object_t *o = data;

    if (o)
        list_del(&o->node);

    if (thiz->destroy)
    {
        thiz->destroy(thiz->ctx, data);
    }
    return 0;
}

vpk_dalist_t *vpk_dalist_create(size_t size, VpkDataDestroy destroy, void *ctx)
{
    vpk_dalist_t *thiz = calloc(1, sizeof(vpk_dalist_t));
    if (thiz)
    {
        INIT_LIST_HEAD(&thiz->head);

        thiz->ctx = ctx;
        thiz->destroy = destroy;
        thiz->array = vpk_darray_create(size, dalist_destroy_data, thiz);
        if (!thiz->array)
            SAFE_FREE(thiz);
    }
    return thiz;
}

int vpk_dalist_add(vpk_dalist_t *thiz, size_t *out_slot, da_object_t *data)
{
    int ret = -1;
    return_val_if_fail(thiz && thiz->array, -1);

    ret = vpk_darray_add(thiz->array, out_slot, (void *)data);
    if (ret == 0)
    {
        list_add_tail(&data->node, &thiz->head);
    }

    return ret;
}

int vpk_dalist_delete(vpk_dalist_t *thiz, size_t slot)
{
    int ret = -1;
    return_val_if_fail(thiz && thiz->array, -1);

    ret = vpk_darray_delete(thiz->array, slot);

    return ret;
}

int vpk_dalist_get_by_index(vpk_dalist_t *thiz, size_t slot, da_object_t **data)
{
    return_val_if_fail(thiz && thiz->array, -1);

    return vpk_darray_get_by_index(thiz->array, slot, (void **)data);
}

//int vpk_dalist_set_by_index(vpk_dalist_t *thiz, size_t slot, da_object_t *data);
int vpk_dalist_modify_by_index(vpk_dalist_t *thiz, size_t slot, VpkDataVisit modify, void *args)
{
    int ret = -1;
    da_object_t *object = NULL;
    return_val_if_fail(thiz && thiz->array, -1);

    ret = vpk_dalist_get_by_index(thiz, slot, &object);
    if (ret == 0)
    {
        ret = modify(args, object);
    }

    return ret;
}

list_t *vpk_dalist_clone_list(vpk_dalist_t *thiz, size_t el_size)
{
    return_val_if_fail(thiz && thiz->array, NULL);

    if (list_empty(&thiz->head))
        return NULL;

    list_t *head = calloc(1, sizeof(list_t));
    if (head)
    {
        INIT_LIST_HEAD(head);

        da_object_t *obj;
        list_t *pos, *n;
        list_for_each_safe(pos, n, &thiz->head)
        {
            obj = container_of(pos, da_object_t, node);
            if (obj)
            {
                da_object_t *new_object = calloc(1, el_size);
                memcpy(new_object, obj, el_size);
                list_add_tail(&new_object->node, head);
            }
        }
    }

    return head;
}

void vpk_dalist_destroy(vpk_dalist_t *thiz)
{
    if (thiz)
    {
        if (thiz->array)
        {
            vpk_darray_destroy(thiz->array);
            thiz->array = NULL;
        }

        if (list_empty(&thiz->head))
            return;

        da_object_t *obj;
        list_t *pos, *n;
        list_for_each_safe(pos, n, &thiz->head)
        {
            obj = container_of(pos, da_object_t, node);
            if (obj)
            {
                list_del(pos);
                thiz->destroy(thiz->ctx, obj);
            }
        }
    }
}

int vpk_dalist_foreach(vpk_dalist_t *thiz, VpkDataVisit visit, void *ctx)
{
    int ret = -1;
    da_object_t *object = NULL;
    return_val_if_fail(thiz && thiz->array, -1);

    list_t *pos, *n;
    list_for_each_safe(pos, n, &thiz->head)
    {
        object = container_of(pos, da_object_t, node);
        if (object)
        {
            ret = visit(ctx, object);
        }
    }

    return ret;
}

size_t vpk_dalist_count(vpk_dalist_t *thiz)
{
    return_val_if_fail(thiz && thiz->array, 0);

    return vpk_darray_count(thiz->array);
}
