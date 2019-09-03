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
#define vpk_timerclear(tvp) timerclear(tvp)
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
#define	vpk_timerclear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0
#endif /* !VPK_HAVE_TIMERADD */

/** 
 * Return true if the tvp is related to uvp according to the relational
 * operator cmp.  Recognized values for cmp are ==, <=, <, >=, and >. 
 */
#define	vpk_timercmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))

#define vpk_gettimeofday(tv, tz)  gettimeofday((tv), (tz))

#define TO_HEX_CHAR(c)	((c) > 9 ? (c) + 55 : (c) + 48)

#define HEX_PARSE_DIGIT_STR(ptr)	\
	((vpk_hex_to_int(*(ptr)) << 12) | (vpk_hex_to_int(*((ptr)+1)) << 8) \
	| (vpk_hex_to_int(*((ptr)+2)) << 4) | vpk_hex_to_int(*((ptr)+3)))

VPKAPI int vpk_hex_to_int(char c);

/**
 * vpk_strcntstr - Get the number of substring in a terminated string
 * @s1: The string to be searched
 * @s2: The string to search for..
 */
VPKAPI int vpk_strcntstr(const char *s1, const char *s2);

VPKAPI void vpk_snprintf(char *buf, unsigned int *pos, size_t len, const char *format, ...);

VPKAPI char* vpk_strstrip(char* str, char c);

VPKAPI char *vpk_bcdtostr(char *buf, unsigned char *bcd, int length, int flag);

#define VPK_HAVE_GETTIMEOFDAY
#ifdef VPK_HAVE_GETTIMEOFDAY
#define vpk_gettimeofday(tv, tz)  gettimeofday((tv), (tz))
#else
struct timezone;
VPKAPI int vpk_gettimeofday(struct timeval *tv, struct timezone *tz);
#endif


static INLINE unsigned int hash_int32(unsigned int a)
{
	a = a ^ (a>>4);
	a = (a^0xdeadbeef) + (a<<5);
	a = a ^ (a>>11);
	return a;
}

TIMA_END_DELS

#endif //VPK_UTIL_H
