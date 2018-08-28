/*
 * History:
 * ================================================================
 * 2017-03-27 qing.zou created
 *
 */
#ifndef GENERIC_NVTUCTRL_H
#define GENERIC_NVTUCTRL_H

#include "vpk_actuator.h"
#include "vpk_session.h"
#include "vpk_actions.h"

TIMA_BEGIN_DELS

const session_ops* get_generic_actuator_ops(void);

TIMA_END_DELS

#endif //GENERIC_NVTUCTRL_H
