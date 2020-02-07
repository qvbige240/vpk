/**
 * History:
 * ================================================================
 * 2020-02-06 qing.zou created
 *
 */

#include "vpk_libc.h"

/* Byte-wise swap two items of size SIZE. */
#define SWAP(a, b, size)             \
    do                               \
    {                                \
        size_t __size = (size);      \
        char *__a = (a), *__b = (b); \
        do                           \
        {                            \
            char __tmp = *__a;       \
            *__a++ = *__b;           \
            *__b++ = __tmp;          \
        } while (--__size > 0);      \
    } while (0)

/* 
 * Sort NMEMB elements of BASE, of SIZE bytes each,
 * using COMPARE to perform the comparisons.
 */
void selection_sort(void *base, size_t nsize, size_t el_size, compare_func_t cmp)
{
    char *start_i = NULL, *start_j = NULL;

    for (start_i = (char *)base; start_i < (char *)base + nsize * el_size; start_i += el_size)
    {
        for (start_j = start_i + el_size; start_j < (char *)base + nsize * el_size; start_j += el_size)
        {
            if ((*cmp)(start_i, start_j) < 0)
            {
                SWAP(start_i, start_j, el_size);
            }
        }
    }
}

static void merge_data(void *base, size_t left, size_t mid, size_t right,
                       size_t el_size, void *tmp, compare_func_t cmp)
{
    size_t index = 0;
    size_t i = left;
    size_t j = mid + 1;

    while (i <= mid && j <= right)
    {
        char *a = (char *)base + i * el_size;
        char *b = (char *)base + j * el_size;
        if ((*cmp)(a, b) <= 0)
        {
            memcpy((char *)tmp + index * el_size, a, el_size);
            index++;
            i++;
        }
        else
        {
            memcpy((char *)tmp + index * el_size, b, el_size);
            index++;
            j++;
        }
    }

    while (i <= mid)
    {
        memcpy((char *)tmp + index * el_size, (char *)base + i * el_size, el_size);
        index++;
        i++;
    }

    while (j <= right)
    {
        memcpy((char *)tmp + index * el_size, (char *)base + j * el_size, el_size);
        index++;
        j++;
    }

    memcpy((char *)base + left * el_size, tmp, index * el_size);
}

void merge_sort(void *base, size_t left, size_t right, size_t el_size, void *tmp, compare_func_t cmp)
{
    if (left >= right)
        return;

    int mid = left + ((right - left) >> 1);
    merge_sort(base, left, mid, el_size, tmp, cmp);
    merge_sort(base, mid + 1, right, el_size, tmp, cmp);

    merge_data(base, left, mid, right, el_size, tmp, cmp);
}

/* 
 * Do a binary search for KEY in BASE, which consists of NMEMB elements
 * of SIZE bytes each, using COMPAR to perform the comparisons.
 */
void *vpk_bsearch(const void *key, const void *base, size_t nmemb, size_t size, compare_func_t cmp)
{
    size_t l, u, idx;
    const void *p;
    int comparison;

    l = 0;
    u = nmemb;
    while (l < u)
    {
        idx = (l + u) / 2;
        p = (void *)(((const char *)base) + (idx * size));
        comparison = (*cmp)(key, p);
        if (comparison < 0)
            u = idx;
        else if (comparison > 0)
            l = idx + 1;
        else
            return (void *)p;
    }

    return NULL;
}
