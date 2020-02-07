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
