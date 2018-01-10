/**
 * History:
 * ================================================================
 * 2017-08-30 qing.zou created
 *
 */
#ifndef TIMA_TYPEDEF_H
#define TIMA_TYPEDEF_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
# define TIMA_EXTERN_BEGIN extern "C" {
# define TIMA_EXTERN_END }
#else
# define TIMA_EXTERN_BEGIN
# define TIMA_EXTERN_END
#endif

//#define MAX_PATH_SIZE				256
//#define MAX_ACTION_RECEIVE_SIZE		256

//#define ZERO_LEN_ARRAY	1
#ifndef return_if_fail
# define return_if_fail(p) if(!(p)) \
	{printf("%s:%d Error: "#p" failed.\n", \
	__FILE__, __LINE__); return;}
# define return_val_if_fail(p, ret) if(!(p)) \
	{printf("%s:%d Error: "#p" failed.\n", \
	__FILE__, __LINE__); return (ret);}
#endif
//#define DECL_PRIV(thiz, priv) PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv : NULL

#ifndef INLINE
# define INLINE __inline__
#else
//#define INLINE
#endif

#ifndef _countof
# define _countof(a)  (sizeof(a)/sizeof(a[0]))
#endif

#ifndef min
# define min(x, y)	((x) < (y) ? (x) : (y))
#endif

#ifndef max
# define max(x, y)	((x) > (y) ? (x) : (y))
#endif

#define TIMAPI			extern	

#endif //TIMA_TYPEDEF_H
