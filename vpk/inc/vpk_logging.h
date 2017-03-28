/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_LOGGING_H
#define VPK_LOGGING_H

//#include "debug.h"
#include "vpk_typedef.h"

TIMA_BEGIN_DELS

/**
 * Set The logging's level.
 *
 * @level  All level: "FATAL", "ERROR", "WARN", "INFO", "DEBUG".
 *
 */
VPKAPI void vpk_logging_level_set(const char* level);

/**
 * Get The logging's level tag.
 *
 * @return  Return level tag: "FATAL", "ERROR", "WARN", "INFO", "DEBUG".
 */
VPKAPI const char* vpk_logging_level_get(void);
VPKAPI void vpk_logging_printf(const char* level, const char* file, int line, const char* format, ...);

#if 1
#define LOG_D(...)	vpk_logging_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_I(...)	vpk_logging_printf("INFO",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_W(...)	vpk_logging_printf("WARN",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_E(...)	vpk_logging_printf("ERROR", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_F(...)	vpk_logging_printf("FATAL", __FILE__, __LINE__, __VA_ARGS__)
#else
/* use zlog */
#define LOG_D(format, args...) vpk_logging_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_I(format, args...) LOGINFO(format"\n", ##args)
#define LOG_W(format, args...) LOGWARN(format"\n", ##args)
#define LOG_E(format, args...) LOGERROR(format"\n", ##args)
#define LOG_F(format, args...) LOGFATAL(format"\n", ##args)
#endif

TIMA_END_DELS

#endif //VPK_LOGGING_H
