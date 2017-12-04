/**
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#ifndef VPK_TYPEDEF_H
#define VPK_TYPEDEF_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#define TIMA_BEGIN_DELS extern "C" {
#define TIMA_END_DELS }
#else
#define TIMA_BEGIN_DELS
#define TIMA_END_DELS
#endif

#define MAX_PATH_SIZE				256
#define MAX_ACTION_RECEIVE_SIZE		256

#define ZERO_LEN_ARRAY	1
#define return_if_fail(p) if(!(p)) \
	{printf("%s:%d Error: "#p" failed.\n", __FILE__, __LINE__); \
		(void)fprintf(stderr, "%s:%d Error: "#p" failed.\n", \
		__FILE__, __LINE__); return;}
#define return_val_if_fail(p, ret) if(!(p)) \
	{printf("%s:%d Error: "#p" failed.\n", __FILE__, __LINE__); \
		(void)fprintf(stderr, "%s:%d Error: "#p" failed.\n", \
		__FILE__, __LINE__); return (ret);}
#define DECL_PRIV(thiz, priv) PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv : NULL

#ifndef INLINE
#define INLINE __inline__
#else
//#define INLINE
#endif

//typedef int bool;
#define false	0
#define true	1

#ifndef _countof
#define _countof(a)  (sizeof(a)/sizeof(a[0]))
#endif

#ifndef min
#define min(x, y)	((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y)	((x) > (y) ? (x) : (y))
#endif

typedef enum _ret
{
	RET_OK = 0,
	RET_FAIL,
	RET_REMOVE,
	RET_CONTINUE,
	RET_FOUND,
	RET_EOF,
	RET_NEXT,
	RET_QUIT,
	RET_EXIST,
	RET_AGAIN,
	RET_IGNORED,
	RET_NO_TARGET,
	RET_NOT_FOUND,
	RET_OUT_OF_SPACE
}Ret;

#define	TIMA_MALLOC		malloc
#define	TIMA_CALLOC		calloc
#define	TIMA_FREE		free
#define	TIMA_MEMSET		memset
#define	TIMA_MEMCPY		memcpy
#define TIMA_REALLOC	realloc

#define	VPK_MALLOC		malloc
#define	VPK_CALLOC		calloc
#define	VPK_FREE		free
#define	VPK_MEMSET		memset
#define	VPK_MEMCPY		memcpy
#define	VPK_REALLOC		realloc

#define VPKAPI			extern	

#endif //VPK_TYPEDEF_H
