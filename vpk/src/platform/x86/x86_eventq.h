/**
 * History:
 * ================================================================
 * 2017-04-12 qing.zou created
 *
 */
#ifndef X86_EVENTQ_H
#define X86_EVENTQ_H

#include "vpk_ieventq.h"

TIMA_BEGIN_DELS

const eventq_ops* get_x86_eventq_ops(void);

TIMA_END_DELS

#endif //X86_EVENTQ_H
