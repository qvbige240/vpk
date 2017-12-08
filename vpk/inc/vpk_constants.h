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
	unsigned int NOTICE;
	//...
}vpk_event_type_t;

typedef struct vpk_keycodes_t
{
#define KEY_EVENT(NAME, TYPE, FIELD, VALUE, READABLE)	unsigned int FIELD;
#include "macro_event.h"
	// KEY_EVENT(SYS_ACC_ON, VPK_EVENT_NOTICE, EVENT_SYS_ACC_ON, 0x1001, "acc on")
#undef KEY_EVENT
	/*
	// NOTICE
	unsigned int EVENT_SYS_ACC_ON;
	unsigned int EVENT_SYS_ACC_OFF;
	unsigned int EVENT_SYS_FAST_ACC;
	unsigned int EVENT_SYS_FAST_DEC;
	unsigned int EVENT_SYS_FAST_TURN;
	unsigned int EVENT_SYS_WAKE_UP_KEY;
	unsigned int EVENT_SYS_WAKE_UP_ACC;
	unsigned int EVENT_SYS_WAKE_UP_GSENSOR;
	unsigned int EVENT_SYS_WAKE_UP_4G;
	unsigned int EVENT_SYS_POWER_OFF_KEY;
	unsigned int EVENT_SYS_POWER_OFF_ACC;
	unsigned int EVENT_SYS_POWER_OFF_GSENSOR;
	unsigned int EVENT_SYS_POWER_OFF_TIMA;
	unsigned int EVENT_SYS_POWER_OFF_VOLTAGE_LOW;
	unsigned int EVENT_QRCODE_GEN;
	unsigned int EVENT_FIRMWARE_DOWNLOAD;
	unsigned int EVENT_UPDATE_SYSTEM;
	unsigned int EVENT_DEVICE_UNBIND;
	unsigned int EVENT_GET_CUR_VIDEO_SUCCESS;
	unsigned int EVENT_GET_CUR_VIDEO_FAILED;
	unsigned int EVENT_CAR_CRASH_NOTICE;		// for car crash notice
	unsigned int EVENT_PARKING_CRASH_NOTICE;	// for warning notice
	// ALERT
	unsigned int EVENT_CAR_CRASH_WARNING;
	unsigned int EVENT_PARKING_CRASH_WARNING;
	// EXCEPTION
	unsigned int EVENT_NO_TF_CARD;
	unsigned int EVENT_TF_CARD_WRITE_PROTECT;
	unsigned int EVENT_TF_CARD_FULL;
	unsigned int EVENT_FILE_READ_ERROR;
	unsigned int EVENT_MEMORY_ERROR;
	unsigned int EVENT_TF_CARD_WR_ERROR;
	unsigned int EVENT_TF_CARD_MIN_ERROR;
	unsigned int EVENT_REAR_SENSOR_REMOVE;
	unsigned int EVENT_FILE_NOT_EXIST;

	// NOTICE
	unsigned int EVENT_UPDATE_SYS_TIME;

	//unsigned int EVENT_POLICE_COME_IN;
	//unsigned int EVENT_CAR_FAST_ACCEL;
	//unsigned int EVENT_CAR_FAST_SLOW_DOWN;
	//unsigned int EVENT_CAR_RAPID_TURN;
	*/
}vpk_keycodes_t;

typedef struct vpk_constants_t
{
	vpk_event_type_t	events;
	vpk_keycodes_t		keys;
}vpk_constants_t;

VPKAPI const vpk_constants_t vpk;

TIMA_END_DELS

#endif //VPK_CONSTANTS_H
