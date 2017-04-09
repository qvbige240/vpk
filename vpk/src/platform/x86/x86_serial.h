/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef X86_SERIAL_H
#define X86_SERIAL_H

#include "vpk_serial.h"
#include "vpk_session.h"

TIMA_BEGIN_DELS

const session_ops* get_x86_serial_ops(void);

TIMA_END_DELS

#endif //X86_SERIAL_H
