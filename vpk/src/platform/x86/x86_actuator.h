/*
 * History:
 * ================================================================
 * 2018-01-04 qing.zou created
 *
 */
#ifndef X86_ACTUATOR_H
#define X86_ACTUATOR_H

#include "vpk_session.h"
#include "vpk_actuator.h"

TIMA_BEGIN_DELS

const session_ops* get_x86_actuator_ops(void);

TIMA_END_DELS

#endif //X86_ACTUATOR_H
