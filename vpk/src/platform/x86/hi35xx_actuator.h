/*
 * History:
 * ================================================================
 * 2018-01-04 qing.zou created
 *
 */
#ifndef HI35XX_ACTUATOR_H
#define HI35XX_ACTUATOR_H

#include "vpk_actuator.h"
#include "vpk_session.h"
#include "vpk_actions.h"

TIMA_BEGIN_DELS

const session_ops* get_hi35xx_actuator_ops(void);

TIMA_END_DELS

#endif //HI35XX_ACTUATOR_H
