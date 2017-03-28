/*
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#include "vpk_logging.h"

#include <string.h>
#include <stdarg.h>

#include "vpk_system.h"
#include "vpk_logger.h"

static vpk_logger_t* logger = NULL;

int vpk_logger_init(void)
{
	const logger_ops* lops = NULL;

	vpk_system_t* vpk_sys = vpk_system();
	return_val_if_fail(vpk_sys != NULL, -1);

	lops = vpk_sys->sys_kset->get_logger_ops();
	return_val_if_fail(lops != NULL, -1);

	logger = lops->init();
	return_val_if_fail(logger != NULL, -1);
	logger->lops = lops;

	return 0;
}

vpk_logger_t* vpk_logger(void)
{
	return logger;
}

const char* const LEVELS[] = 
{
	"FATAL",
	"ERROR",
	"WARN",
	"INFO",
	"DEBUG",
};

const char* const FORMAT_LEVELS[] = 
{
	"FATAL",
	"ERROR",
	" WARN",
	" INFO",
	"DEBUG",
};

int get_level_value(const char* level)
{
	int i = 0;
	for (i = 0; i < sizeof(LEVELS)/sizeof(LEVELS[0]); i++)
	{
		if (strcmp(LEVELS[i], level) == 0)
		{
			return i;
		}
	}
	return -1;
}

void vpk_logging_level_set(const char* level)
{
	int lv = -1;
	return_if_fail(level != NULL);
	
	lv = get_level_value(level);
	if (lv >= 0 && lv < _countof(LEVELS))
	{
		logger->level = lv;
	}
}

const char* vpk_logging_level_get(void)
{
	return_val_if_fail(logger != NULL, NULL);
	return LEVELS[logger->level];
}

void vpk_logging_printf(const char* level, const char* file, int line, const char* format, ...)
{
	int lv = -1;
	return_if_fail(level != NULL && file != NULL);

	lv = get_level_value(level);
	if (lv >= 0 && lv < _countof(LEVELS) && logger && lv <= logger->level)
	{
		char message[256] = {0};
		va_list va;

		va_start(va, format);
		vsnprintf(message, sizeof(message)-1, format, va);
		va_end(va);

		logger->lops->write(logger, lv, file, line, message);
	}
}

