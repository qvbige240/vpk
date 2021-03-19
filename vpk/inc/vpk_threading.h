/**
 * History:
 * ================================================================
 * 2020-12-05 qing.zou created
 *
 */
#ifndef VPK_THREADING_H
#define VPK_THREADING_H

#include <stdbool.h>
#include <pthread.h>

#include "vpk_typedef.h"

VPK_BEGIN_DELS

static INLINE long vpk_atomic_inc_long(volatile long *val)
{
    return __sync_add_and_fetch(val, 1);
}

static INLINE long vpk_atomic_dec_long(volatile long *val)
{
    return __sync_sub_and_fetch(val, 1);
}

static INLINE long vpk_atomic_set_long(volatile long *ptr, long val)
{
    return __sync_lock_test_and_set(ptr, val);
}

static INLINE long vpk_atomic_load_long(const volatile long *ptr)
{
    return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

static INLINE bool vpk_atomic_compare_swap_long(volatile long *val, long old_val, long new_val)
{
    return __sync_bool_compare_and_swap(val, old_val, new_val);
}

static INLINE bool vpk_atomic_set_bool(volatile bool *ptr, bool val)
{
    return __sync_lock_test_and_set(ptr, val);
}

static INLINE bool vpk_atomic_load_bool(const volatile bool *ptr)
{
    return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

VPK_END_DELS

#endif // VPK_THREADING_H
