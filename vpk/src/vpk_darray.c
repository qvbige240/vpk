/**
 * History:
 * ================================================================
 * 2019-09-23 qing.zou created
 *
 */

#include "vpk_darray.h"

static void darray_destroy_data(vpk_darray_t *thiz, void *data)
{
    if (thiz->destroy)
    {
        thiz->destroy(thiz->ctx, data);
    }
}

vpk_darray_t *vpk_darray_create(size_t size, VpkDataDestroy destroy, void *ctx)
{
    vpk_darray_t *thiz = calloc(1, sizeof(vpk_darray_t));
    if (thiz)
    {
        thiz->size      = 0;
        thiz->capacity  = size;
        thiz->cursor    = 0;
        thiz->destroy   = destroy;
        thiz->ctx       = ctx;
        thiz->data = calloc(1, sizeof(void *) * size);
        if (thiz->data == NULL)
        {
            free(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

int vpk_darray_expand(vpk_darray_t *thiz, size_t need)
{
    return_val_if_fail(thiz, -1);

    size_t capacity = thiz->capacity;

    while ((thiz->size + need) >= capacity)
    {
        capacity = capacity + (capacity >> 1);
    }

    void **data = realloc(thiz->data, capacity * sizeof(void *));
    if (data == NULL)
    {
        printf("realloc memory failed\n");
        return -1;
    }
    memset((void *)data + thiz->capacity * sizeof(void *), 0x00, (capacity - thiz->capacity) * sizeof(void *));
    thiz->data = data;
    thiz->capacity = capacity;

    return 0;
}

//int vpk_darray_insert(vpk_darray_t *thiz, size_t index, void *data);
int vpk_darray_add(vpk_darray_t *thiz, size_t *out_index, void *data)
{
    size_t next_cursor;
    return_val_if_fail(thiz, -1);

    size_t need = 1;
    if ((thiz->size + need) >= thiz->capacity)
    {
        if (vpk_darray_expand(thiz, need) == -1)
        {
            printf("darray expand failed\n");
            return -1;
        }
    }

    if (out_index)
        *out_index = thiz->cursor;

    if (thiz->tails == thiz->size)
    {
        thiz->data[thiz->cursor] = data;
        thiz->tails++;
        thiz->size++;
        thiz->cursor++;
        printf("size: %d, tails: %d\n", thiz->size, thiz->tails);
    }
    else
    {
        next_cursor = ((void *)thiz->data[thiz->cursor] - (void *)thiz->data) / sizeof(void *);
        printf("thiz cursor %d, next cursor: %d [%p-%p=%d]\n",
               thiz->cursor, next_cursor, thiz->data[thiz->cursor], thiz->data, (int)((void **)thiz->data[thiz->cursor] - thiz->data));
        thiz->data[thiz->cursor] = data;
        thiz->cursor = next_cursor;
        thiz->size++;
    }

    // if (thiz->tails == thiz->size)
    // {
    //     thiz->tails++;
    //     next_cursor = thiz->cursor;
    //     next_cursor++;
    // }
    // else
    // {
    //     next_cursor = ((void**)thiz->data[thiz->cursor] - thiz->data)/sizeof(void*);
    // }

    // thiz->data[thiz->cursor] = data;
    // thiz->size++;
    // thiz->cursor = next_cursor;

    // if (thiz->cursor == thiz->size)
    //     next_cursor = thiz->cursor++;
    // else
    // {
    //     next_cursor = (size_t)thiz->data[thiz->cursor];
    //     // void *next = thiz->data[thiz->cursor];
    //     // if (next)
    //     //     next_cursor = (size_t)next;
    //     // else
    //     //     next_cursor = thiz->size;
    // }

    // thiz->data[thiz->cursor] = data;
    // thiz->size++;
    // thiz->cursor = next_cursor;

    return 0;
}

static int valid_index(vpk_darray_t *thiz, size_t index)
{
    void **value = thiz->data[index];
    void **range_end = (void **)(&thiz->data[thiz->tails + 1]);

    if ((value >= thiz->data && value < range_end) || (index >= thiz->tails))
        return 0;

    return 1;
}

int vpk_darray_delete(vpk_darray_t *thiz, size_t index)
{
    return_val_if_fail(thiz, -1);

    if (index >= thiz->tails)
    {
        printf("darray delete failed: index not used\n");
        return -1;
    }

    if (!valid_index(thiz, index))
    {
        printf("duplicated, darray already have been deleted\n");
        return -2;
    }

    darray_destroy_data(thiz, thiz->data[index]);
    thiz->data[index] = (void *)&(thiz->data[thiz->cursor]);
    printf("cur cursor %d, ", thiz->cursor);
    thiz->cursor = index;
    printf("next cursor %d\n", thiz->cursor);
    thiz->size--;
    // if (index > thiz->capacity)
    // {
    //     printf("darray delete failed: index not exists\n");
    //     return -1;
    // }

    // if (thiz->data[index] < thiz->capacity)  //...
    // {
    //     printf("duplicated, darray already have been deleted\n");
    //     return 1;
    // }

    // darray_destroy_data(thiz, thiz->data[index]);
    // thiz->data[index] = (void *)thiz->cursor;
    // thiz->cursor = index;
    // thiz->size--;

    return 0;
}

int vpk_darray_get_by_index(vpk_darray_t *thiz, size_t index, void **data)
{
    return_val_if_fail(thiz && data && index < thiz->capacity, -1);

    if (!valid_index(thiz, index))
    {
        printf("invalid index %d\n", index);
        *data = thiz->data[index];
        return -2;
    }

    *data = thiz->data[index];

    return 0;
}

int vpk_darray_set_by_index(vpk_darray_t *thiz, size_t index, void *data)
{
    return_val_if_fail(thiz && index < thiz->capacity, -1);

    if (!valid_index(thiz, index))
    {
        printf("invalid index %d\n", index);
        return -2;
    }

    thiz->data[index] = data;

    return 0;
}

size_t vpk_darray_count(vpk_darray_t *thiz)
{
    return_val_if_fail(thiz, 0);

    return thiz->size;
}

int vpk_darray_foreach(vpk_darray_t *thiz, VpkDataVisit visit, void *ctx)
{
    size_t i = 0;
    return_val_if_fail(thiz && visit, -1);
    for (i = 0; i < thiz->capacity; i++)
    {
        if (valid_index(thiz, i))
            visit(ctx, thiz->data[i]);
    }
    return 0;
}

int vpk_darray_find(vpk_darray_t *thiz, VpkDataCompare cmp, void *ctx)
{
    size_t i = 0;
    return_val_if_fail(thiz && cmp, -1);
    for (i = 0; i < thiz->capacity; i++)
    {
        if (valid_index(thiz, i))
            if (cmp(ctx, thiz->data[i]) == 0)
                break;
    }
    return i;
}

void vpk_darray_destroy(vpk_darray_t *thiz)
{
    size_t i = 0;
    if (thiz)
    {
        for (i = 0; i < thiz->capacity; i++)
        {
            if (valid_index(thiz, i))
                darray_destroy_data(thiz, thiz->data[i]);
        }
        SAFE_FREE(thiz->data);
        SAFE_FREE(thiz);
    }
}
