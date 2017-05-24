/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_UTIL_H
#define VPK_UTIL_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

/* VPK_SWAP_VALUE - this swaps x for y */
#define VPK_SWAP_VALUE(x, y)	do { (x) ^= (y); (y) ^= (x); (x) ^= (y); } while(0)

#ifdef VPK__HAVE_TIMERADD
#define vpk_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#define vpk_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#else
#define vpk_timeradd(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#define	vpk_timersub(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)
#endif /* !VPK__HAVE_TIMERADD */

int vpk_hex_to_int(char c);

/**
 * vpk_strcntstr - Get the number of substring in a terminated string
 * @s1: The string to be searched
 * @s2: The string to search for..
 */
int vpk_strcntstr(const char *s1, const char *s2);

TIMA_END_DELS

#endif //VPK_UTIL_H
