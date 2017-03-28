/*
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#ifndef VPK_LOGGER_H
#define VPK_LOGGER_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

typedef struct vpk_logger_t vpk_logger_t;

typedef struct logger_ops_s 
{
	vpk_logger_t* (*init)(void);
	int (*write)(vpk_logger_t* logger, int level, const char* file, int line, const char* message);
}logger_ops;

struct vpk_logger_t
{
	const logger_ops* lops;
	int level;
};

extern const char* const LEVELS[];
extern const char* const FORMAT_LEVELS[] ;

int vpk_logger_init(void);
vpk_logger_t* vpk_logger(void);

int get_level_value(const char* level);
void vpk_logging_level_set(const char* level);
const char* vpk_logging_level_get(void);
void vpk_logging_printf(const char* level, const char* file, int line, const char* format, ...);


TIMA_END_DELS

#endif //VPK_LOGGER_H
