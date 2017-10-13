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

#ifdef USE_ZLOG
#include "zlog.h"
/* use zlog */
#ifndef TIMA_LOGI
/* tima log macros */
#define TIMA_LOGF(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define TIMA_LOGE(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define TIMA_LOGW(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)
#define TIMA_LOGN(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define TIMA_LOGI(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define TIMA_LOGD(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#endif

//#define LOG_D(format, args...) vpk_logging_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
//#define LOG_D(format, args...) TIMA_LOGD(format"\n", ##args)
#define LOG_D(format, args...) TIMA_LOGD(format, ##args)
#define LOG_I(format, args...) TIMA_LOGI(format, ##args)
#define LOG_W(format, args...) TIMA_LOGW(format, ##args)
#define LOG_E(format, args...) TIMA_LOGE(format, ##args)
#define LOG_F(format, args...) TIMA_LOGF(format, ##args)
#else
#define LOG_D(...)	vpk_logging_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_I(...)	vpk_logging_printf("INFO",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_W(...)	vpk_logging_printf("WARN",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_E(...)	vpk_logging_printf("ERROR", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_F(...)	vpk_logging_printf("FATAL", __FILE__, __LINE__, __VA_ARGS__)
#endif

TIMA_END_DELS

#endif //VPK_LOGGING_H
