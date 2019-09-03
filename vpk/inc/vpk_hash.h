/**
 * History:
 * ================================================================
 * 2019-06-25 qing.zou created
 *
 */
#ifndef VPK_HASH_H
#define VPK_HASH_H

#include "vpk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vpk_hash_s {
	int		bucket;
	void	*opaque_list;
	void	(*destory)(void *val);
} vpk_hash_t;

vpk_hash_t *vpk_hash_create(int bucket);
void vpk_hash_destroy(vpk_hash_t *h);
void vpk_hash_set_destory(vpk_hash_t *h, void (*destory)(void *val));

uint32_t vpk_hash_gen32(const char *key, size_t len);
int vpk_hash_exist(vpk_hash_t *h, const char *key);
void *vpk_hash_get(vpk_hash_t *h, const char *key);
int vpk_hash_set(vpk_hash_t *h, const char *key, void *val);
int vpk_hash_del(vpk_hash_t *h, const char *key);
void *vpk_hash_get_and_del(vpk_hash_t *h, const char *key);


#ifdef __cplusplus
}
#endif

#endif // VPK_HASH_H
