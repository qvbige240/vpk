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
	unsigned int ABNORMAL;
	unsigned int ALERT;
	//...
}vpk_event_type_t;

typedef struct vpk_keycodes_t
{
	// ABNORMAL
	unsigned int EVENT_NO_TF_CARD;
	unsigned int EVENT_TF_CARD_WRITE_PROTECT;
	unsigned int EVENT_TF_CARD_FULL;
	unsigned int EVENT_FILE_READ_ERROR;
	unsigned int EVENT_MEMORY_ERROR;
	unsigned int EVENT_TF_CARD_WR_ERROR;
	unsigned int EVENT_TF_CARD_MIN_ERROR;
	unsigned int EVENT_REAR_SENSOR_REMOVE;
	unsigned int EVENT_FILE_NOT_EXIST;
	// ALERT
	unsigned int EVENT_CAR_CRASH_WARNING;
	// ALERT
	unsigned int EVENT_PARKING_CRASH_WARNING;
	unsigned int EVENT_POLICE_COME_IN;
	unsigned int EVENT_CAR_FAST_ACCEL;
	unsigned int EVENT_CAR_FAST_SLOW_DOWN;
	unsigned int EVENT_CAR_RAPID_TURN;
}vpk_keycodes_t;

typedef struct vpk_constants_t
{
	vpk_event_type_t	events;
	vpk_keycodes_t		keys;
}vpk_constants_t;

VPKAPI const vpk_constants_t vpk;

TIMA_END_DELS

#endif //VPK_CONSTANTS_H
