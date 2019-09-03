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

VPK_BEGIN_DELS

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
#ifndef VPK_LOGI
/* tima log macros */
#define VPK_LOGF(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define VPK_LOGE(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define VPK_LOGW(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)
#define VPK_LOGN(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define VPK_LOGI(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define VPK_LOGD(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#endif

//#define LOG_D(format, args...) vpk_logging_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
//#define LOG_D(format, args...) VPK_LOGD(format"\n", ##args)
#define LOG_D(format, args...) VPK_LOGD(format, ##args)
#define LOG_I(format, args...) VPK_LOGI(format, ##args)
#define LOG_W(format, args...) VPK_LOGW(format, ##args)
#define LOG_E(format, args...) VPK_LOGE(format, ##args)
#define LOG_F(format, args...) VPK_LOGF(format, ##args)
#else
#define LOG_D(...)	vpk_logging_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_I(...)	vpk_logging_printf("INFO",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_W(...)	vpk_logging_printf("WARN",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_E(...)	vpk_logging_printf("ERROR", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_F(...)	vpk_logging_printf("FATAL", __FILE__, __LINE__, __VA_ARGS__)
#endif

#ifndef TIMA_LOGI
    #define TIMA_LOGD		LOG_D
    #define TIMA_LOGI		LOG_I
    #define TIMA_LOGW		LOG_W
    #define TIMA_LOGE		LOG_E
    #define TIMA_LOGF		LOG_F
#endif

VPK_END_DELS

#endif //VPK_LOGGING_H
