/**
 * History:
 * ================================================================
 * 2017-04-13 qing.zou created
 *
 */
#ifndef NT966X_EVENTQ_H
#define NT966X_EVENTQ_H

#include "vpk_ieventq.h"

TIMA_BEGIN_DELS

enum {
	NT966X_EVENT_ALERT = 0x00,

	//...
// #define EVENT(NAME) NT966X_EVENT_ ## NAME,
// #include "events.xmacro"
// #undef EVENT
};

enum {
	NT966X_KEY_EVENT_NO_TF_CARD = 0x3001,
	NT966X_KEY_EVENT_TF_CARD_WRITE_PROTECT,
	NT966X_KEY_EVENT_TF_CARD_FULL,

	//...
// #define KEY(NAME, FIELD, READABLE)    NT966X_KEY_ ## NAME,
// #include "keys.xmacro"
// #undef KEY
};

const eventq_ops* get_nt966x_eventq_ops(void);

TIMA_END_DELS

#endif //NT966X_EVENTQ_H
