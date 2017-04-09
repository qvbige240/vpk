/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef X86_LOG_H
#define X86_LOG_H

#include "vpk_logger.h"

TIMA_BEGIN_DELS

const logger_ops* get_x86_logger_ops(void);

TIMA_END_DELS

#endif //X86_LOG_H
