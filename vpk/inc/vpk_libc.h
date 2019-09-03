/**
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#ifndef VPK_LIBC_H
#define VPK_LIBC_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

/* typedef, shorthand for type of comparison functions. */
typedef int (*compare_func_t)(const void *, const void *);

/**
 * Sort NMEMB elements of BASE, of SIZE bytes each,
 * using COMPARE to perform the comparisons. 
 */
VPKAPI void selection_sort(void *base, size_t nsize, size_t el_size, compare_func_t cmp);

/**
 * Sort NMEMB elements of BASE, of SIZE bytes each,
 * using COMPARE to perform the comparisons. 
 *
 * @param base	   The Target object to sort.
 * @param nsize    The total num of pointer object.
 * @param el_size  The data size of object element.
 * @param cmp      The comparison functions.
 *
 */
static INLINE void vpk_ssort(void *base, size_t nsize, size_t el_size, compare_func_t cmp)
{
	selection_sort(base, nsize, el_size, cmp);
}

/**
 * Do a binary search for KEY in BASE, which consists of NMEMB elements
 * of SIZE bytes each, using COMPAR to perform the comparisons.  
 */
VPKAPI void *vpk_bsearch(const void *key, const void *base, size_t nmemb, size_t size, compare_func_t cmp);

/**
 *	system function, number of cores currently available in the system.
 */
VPKAPI size_t vpk_numproc(void);

/**
 *	system call function with time out.
 */
VPKAPI int vpk_system_ex(const char *cmd, unsigned int timout);

/**
 *	system function, exec a command and return a buffer.
 */
VPKAPI int vpk_system_exval(char* cmd, char* buf, int size);

TIMA_END_DELS

#endif/*VPK_LIBC_H*/
