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

enum {
	X86_EVENT_ALERT = 0x00,

	//...
// #define EVENT(NAME) X86_EVENT_ ## NAME,
// #include "events.xmacro"
// #undef EVENT
};

enum {
	X86_KEY_EVENT_NO_TF_CARD = 0x3001,
	X86_KEY_EVENT_TF_CARD_WRITE_PROTECT,
	X86_KEY_EVENT_TF_CARD_FULL,

	//...
// #define KEY(NAME, FIELD, READABLE)    X86_KEY_ ## NAME,
// #include "keys.xmacro"
// #undef KEY
};

const eventq_ops* get_x86_eventq_ops(void);

TIMA_END_DELS

#endif //X86_EVENTQ_H
