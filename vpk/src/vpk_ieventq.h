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
	// ABNORMAL
	VPK_KEY_EVENT_NO_TF_CARD = 0x3001,
	VPK_KEY_EVENT_TF_CARD_WRITE_PROTECT,
	VPK_KEY_EVENT_TF_CARD_FULL,
	VPK_KEY_EVENT_FILE_READ_ERROR,
	VPK_KEY_EVENT_MEMORY_ERROR,
	VPK_KEY_EVENT_TF_CARD_WR_ERROR,
	VPK_KEY_EVENT_TF_CARD_MIN_ERROR,
	VPK_KEY_EVENT_REAR_SENSOR_REMOVE,
	VPK_KEY_EVENT_FILE_NOT_EXIST = 0x3050,
	// ALERT
	VPK_KEY_EVENT_CAR_CRASH_WARNING = 0x3100,
	// ALERT
	VPK_KEY_EVENT_PARKING_CRASH_WARNING = 0x3101,

	VPK_KEY_EVENT_POLICE_COME_IN,
	VPK_KEY_EVENT_CAR_FAST_ACCEL,
	VPK_KEY_EVENT_CAR_FAST_SLOW_DOWN = 0x3014,
	VPK_KEY_EVENT_CAR_RAPID_TURN,

	// NOTICE
	VPK_KEY_EVENT_QRCODE_GEN,
	VPK_KEY_EVENT_FIRMWARE_DOWNLOAD,
	VPK_KEY_EVENT_UPDATE_SYSTEM,

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
