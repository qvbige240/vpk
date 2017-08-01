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

#ifdef VPK_HAVE_TIMERADD
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
#endif /* !VPK_HAVE_TIMERADD */

/** 
 * Return true if the tvp is related to uvp according to the relational
 * operator cmp.  Recognized values for cmp are ==, <=, <, >=, and >. 
 */
#define	vpk_timercmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))


#define TO_HEX_CHAR(c)	((c) > 9 ? (c) + 55 : (c) + 48)

VPKAPI int vpk_hex_to_int(char c);

/**
 * vpk_strcntstr - Get the number of substring in a terminated string
 * @s1: The string to be searched
 * @s2: The string to search for..
 */
VPKAPI int vpk_strcntstr(const char *s1, const char *s2);

VPKAPI char* vpk_strstrip(char* str, char c);

#define VPK_HAVE_GETTIMEOFDAY
#ifdef VPK_HAVE_GETTIMEOFDAY
#define vpk_gettimeofday(tv, tz)  gettimeofday((tv), (tz))
#else
struct timezone;
int vpk_gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

TIMA_END_DELS

#endif //VPK_UTIL_H
