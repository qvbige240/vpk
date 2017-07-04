/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "x86_log.h"

static vpk_logger_t x86_logger = {0};
static const logger_ops x86_logger_ops;

static vpk_logger_t* x86_logger_init(void)
{
	vpk_logger_t* thiz = &x86_logger;
	if (thiz)
	{
		thiz->lops = &x86_logger_ops;
#if defined(DEBUG)
		thiz->level = 4;
#else
		thiz->level = 2;                                        /* level: "WARN" */
#endif
	}

	return thiz;
}

static int x86_logger_write(vpk_logger_t* logger, int level, const char* file, int line, const char* message)
{
	logger = logger;
	printf("%s[%d] %s: %s\n", file, line, FORMAT_LEVELS[level], message);
	//dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, ZLOG_LEVEL_WARN, format, ##args)

	return 0;
}

static const logger_ops x86_logger_ops = 
{
	x86_logger_init,
	x86_logger_write,
};

const logger_ops* get_x86_logger_ops(void)
{
	return &x86_logger_ops;
}

