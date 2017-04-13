/**
 * History:
 * ================================================================
 * 2017-04-12 qing.zou created
 *
 */
#ifndef VPK_CONSTANTS_H
#define VPK_CONSTANTS_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

typedef struct vpk_event_type_t
{
	unsigned int ALERT;
	//...
}vpk_event_type_t;

typedef struct vpk_keycodes_t
{
	unsigned int EVENT_NO_TF_CARD;
	unsigned int EVENT_TF_CARD_WRITE_PROTECT;
	unsigned int EVENT_TF_CARD_FULL;
	//...
}vpk_keycodes_t;

typedef struct vpk_constants_t
{
	vpk_event_type_t	events;
	vpk_keycodes_t		keys;
}vpk_constants_t;

VPKAPI const vpk_constants_t vpk;

TIMA_END_DELS

#endif //VPK_CONSTANTS_H
