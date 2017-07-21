/**
 * History:
 * ================================================================
 * 2017-04-11 qing.zou created
 *
 */
#ifndef VPK_IEVENTQ_H
#define VPK_IEVENTQ_H

#include "vpk_event.h"
#include "vpk_typedef.h"
#include "vpk_object.h"

TIMA_BEGIN_DELS


enum {
	VPK_EVENT_ABNORMAL = 0x10,
	VPK_EVENT_ALERT,
	VPK_EVENT_NOTICE,

	//...
	// #define EVENT(NAME) VPK_EVENT_ ## NAME,
	// #include "events.xmacro"
	// #undef EVENT
};

enum {
	// NOTICE
	VPK_KEY_EVENT_SYS_ACC_ON = 0x1001,
	VPK_KEY_EVENT_SYS_ACC_OFF,
	VPK_KEY_EVENT_SYS_FAST_ACC,
	VPK_KEY_EVENT_SYS_FAST_DEC,
	VPK_KEY_EVENT_SYS_FAST_TURN,

	VPK_KEY_EVENT_SYS_WAKE_UP_KEY = 0x1030,
	VPK_KEY_EVENT_SYS_WAKE_UP_ACC,
	VPK_KEY_EVENT_SYS_WAKE_UP_GSENSOR,
	VPK_KEY_EVENT_SYS_WAKE_UP_4G,
	VPK_KEY_EVENT_SYS_POWER_OFF_KEY = 0x1040,
	VPK_KEY_EVENT_SYS_POWER_OFF_ACC,
	VPK_KEY_EVENT_SYS_POWER_OFF_GSENSOR,
	VPK_KEY_EVENT_SYS_POWER_OFF_TIMA,
	// NOTICE
	VPK_KEY_EVENT_QRCODE_GEN = 0x1050,
	VPK_KEY_EVENT_FIRMWARE_DOWNLOAD,
	VPK_KEY_EVENT_UPDATE_SYSTEM,
	// NOTICE
	VPK_KEY_EVENT_GET_CUR_VIDEO_SUCCESS = 0x1100,
	VPK_KEY_EVENT_GET_CUR_VIDEO_FAILED,
	// ALERT
	VPK_KEY_EVENT_CAR_CRASH_WARNING	= 0x2001,
	VPK_KEY_EVENT_PARKING_CRASH_WARNING,

	// EXCEPTION
	VPK_KEY_EVENT_NO_TF_CARD = 0x3001,
	VPK_KEY_EVENT_TF_CARD_WRITE_PROTECT,
	VPK_KEY_EVENT_TF_CARD_FULL,
	VPK_KEY_EVENT_FILE_READ_ERROR,
	VPK_KEY_EVENT_MEMORY_ERROR,
	VPK_KEY_EVENT_TF_CARD_WR_ERROR,
	VPK_KEY_EVENT_TF_CARD_MIN_ERROR,
	VPK_KEY_EVENT_REAR_SENSOR_REMOVE,
	VPK_KEY_EVENT_FILE_NOT_EXIST,

	// hwclock -s -u
	VPK_KEY_EVENT_UPDATE_SYS_TIME = 0x8001,

	// #define KEY(NAME, FIELD, READABLE)    VPK_KEY_ ## NAME,
	// #include "keys.xmacro"
	// #undef KEY
};


typedef struct eventq_ops_s
{
	vpk_eventq_t*	(*init)(const char* name, const char* mode);
	int				(*empty)(const vpk_eventq_t* queue);
	int				(*recv)(vpk_eventq_t *queue, vpk_event_t* e);
	int				(*post)(vpk_eventq_t *queue, vpk_event_t* e);
	int				(*close)(vpk_eventq_t *queue);
}eventq_ops;

#define extends_eventq()						\
	extends_object();							\
	int fd; 									\
	char name[32];								\
	char recv_buff[EVENT_MQ_MSG_LEN_MAX];		\
	const eventq_ops* e_ops

struct vpk_eventq_t
{
	extends_eventq();
};

int vpk_eventq_init(void);
// vpk_eventq_t* vpk_eventq(void);

TIMA_END_DELS

#endif //VPK_IEVENTQ_H
