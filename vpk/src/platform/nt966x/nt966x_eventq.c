/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include "nt966x_eventq.h"
#include "vpk_event.h"
#include "vpk_logging.h"
#include "vpk_util.h"

#include "nvtevent.h"

// //customed event from uITRON
// #define CUSTOM_EVT_PREFIX				"CST,"
// #define CUSTOM_EVT_MQ_MSG_NUM_MAX		10
// #define CUSTOM_EVT_MQ_MSG_LEN_MAX		12
// #define CUSTOM_NOTIFY_EVENT_QUEUE_NAME	"/NvtCustomEventMq"

#include "vpk_constants.h"

const vpk_constants_t vpk = {
	{
		VPK_EVENT_ABNORMAL,
		VPK_EVENT_ALERT,
		VPK_EVENT_NOTICE,
	},
	{
		// NOTICE
		VPK_KEY_EVENT_SYS_ACC_ON,
		VPK_KEY_EVENT_SYS_ACC_OFF,
		VPK_KEY_EVENT_SYS_FAST_ACC,
		VPK_KEY_EVENT_SYS_FAST_DEC,
		VPK_KEY_EVENT_SYS_FAST_TURN,

		VPK_KEY_EVENT_SYS_WAKE_UP_KEY,
		VPK_KEY_EVENT_SYS_WAKE_UP_ACC,
		VPK_KEY_EVENT_SYS_WAKE_UP_GSENSOR,
		VPK_KEY_EVENT_SYS_WAKE_UP_4G,
		VPK_KEY_EVENT_SYS_POWER_OFF_KEY,
		VPK_KEY_EVENT_SYS_POWER_OFF_ACC,
		VPK_KEY_EVENT_SYS_POWER_OFF_GSENSOR,
		VPK_KEY_EVENT_SYS_POWER_OFF_TIMA,
		// NOTICE
		VPK_KEY_EVENT_QRCODE_GEN,
		VPK_KEY_EVENT_FIRMWARE_DOWNLOAD,
		VPK_KEY_EVENT_UPDATE_SYSTEM,

		VPK_KEY_EVENT_GET_CUR_VIDEO_SUCCESS,
		VPK_KEY_EVENT_GET_CUR_VIDEO_FAILED,
		// ALERT
		VPK_KEY_EVENT_CAR_CRASH_WARNING,
		VPK_KEY_EVENT_PARKING_CRASH_WARNING,
		// EXCEPTION
		VPK_KEY_EVENT_NO_TF_CARD,
		VPK_KEY_EVENT_TF_CARD_WRITE_PROTECT,
		VPK_KEY_EVENT_TF_CARD_FULL,
		VPK_KEY_EVENT_FILE_READ_ERROR,
		VPK_KEY_EVENT_MEMORY_ERROR,
		VPK_KEY_EVENT_TF_CARD_WR_ERROR,
		VPK_KEY_EVENT_TF_CARD_MIN_ERROR,
		VPK_KEY_EVENT_REAR_SENSOR_REMOVE,
		VPK_KEY_EVENT_FILE_NOT_EXIST,
		// NOTICE
		VPK_KEY_EVENT_UPDATE_SYS_TIME,
	},
};

static int nt966x_eventq_destruct(void *queue)
{
	vpk_eventq_t* thiz = (vpk_eventq_t*)queue;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("eventq \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			mq_close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->e_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_eventq_t* nt966x_eventq_init(const char* name, const char* mode)
{
	int rw, oflags, create = 0;
	vpk_eventq_t* thiz = NULL;
	struct mq_attr attr;
	attr.mq_flags	= O_NONBLOCK;
	attr.mq_maxmsg	= CUSTOM_EVT_MQ_MSG_NUM_MAX;
	attr.mq_msgsize = CUSTOM_EVT_MQ_MSG_LEN_MAX;
	attr.mq_curmsgs = 0;

	rw = (mode[1] == '+') || (mode[1] && (mode[2] == '+'));

	switch (*mode)
	{
	case 'a':
		oflags = O_CREAT  | (rw ? O_RDWR : O_WRONLY);
		create = 1;
		break;
	case 'r':
		oflags = rw ? O_RDWR : O_RDONLY;
		break;
	case 'w':
		//oflags = O_CREAT | O_TRUNC | (rw ? O_RDWR : O_WRONLY);
		//oflags = O_EXCL | (rw ? O_RDWR : O_WRONLY);
		oflags = rw ? O_RDWR : O_WRONLY;
		break;
	default:
		return (NULL);
	}

	thiz = (vpk_eventq_t*)TIMA_MALLOC(sizeof(vpk_eventq_t));
	if (thiz)
	{
		memset(thiz, 0x00, sizeof(*thiz));
		thiz->destruct	= nt966x_eventq_destruct;
		thiz->fd		= -1;
		memcpy(thiz->name, CUSTOM_NOTIFY_EVENT_QUEUE_NAME, sizeof(thiz->name));		
		if (create)
			thiz->fd = mq_open(CUSTOM_NOTIFY_EVENT_QUEUE_NAME, oflags, 0666, &attr);
		else
			thiz->fd = mq_open(CUSTOM_NOTIFY_EVENT_QUEUE_NAME, oflags);
		if (thiz->fd < 0)
		{
			LOG_E("mq_open error(name = %s), fd: %d", thiz->name, thiz->fd);
			TIMA_FREE(thiz);
			return NULL;
		}
		LOG_D("eventq(name = \'%s\') opened, fd = %d", thiz->name, thiz->fd);
	}

	return thiz;
}

static int nt966x_eventq_empty(const vpk_eventq_t* queue)
{
	return 0;
}

#define HEXSTR_PARSE(ptr)	\
	((vpk_hex_to_int(*(ptr)) << 12) | (vpk_hex_to_int(*((ptr)+1)) << 8) \
	| (vpk_hex_to_int(*((ptr)+2)) << 4) | vpk_hex_to_int(*((ptr)+3)))

static int nt966x_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e)
{
	int len = 0;
	vpk_eventq_t* thiz = queue;
	return_val_if_fail(thiz != NULL && e != NULL, -1);

	memset(thiz->recv_buff, 0x00, sizeof(thiz->recv_buff));
	len = mq_receive(thiz->fd, thiz->recv_buff, CUSTOM_EVT_MQ_MSG_LEN_MAX, NULL);
	if (len > 0) 
	{
		if (len >= CUSTOM_EVT_MQ_MSG_LEN_MAX)
			thiz->recv_buff[CUSTOM_EVT_MQ_MSG_LEN_MAX-1] = '\0';

		//int keycode = atoi(thiz->recv_buff);
		int keycode = HEXSTR_PARSE(&thiz->recv_buff[0]);
		switch (keycode)
		{
		case VPK_KEY_EVENT_NO_TF_CARD:
		case VPK_KEY_EVENT_TF_CARD_WRITE_PROTECT:
		case VPK_KEY_EVENT_TF_CARD_FULL:
		case VPK_KEY_EVENT_FILE_READ_ERROR:
		case VPK_KEY_EVENT_MEMORY_ERROR:
		case VPK_KEY_EVENT_TF_CARD_WR_ERROR:
		case VPK_KEY_EVENT_TF_CARD_MIN_ERROR:
		case VPK_KEY_EVENT_REAR_SENSOR_REMOVE:
		case VPK_KEY_EVENT_FILE_NOT_EXIST:
		//case VPK_KEY_EVENT_POLICE_COME_IN:
		//case VPK_KEY_EVENT_CAR_FAST_ACCEL:
		//case VPK_KEY_EVENT_CAR_FAST_SLOW_DOWN:
		//case VPK_KEY_EVENT_CAR_RAPID_TURN:
			e->type = vpk.events.ABNORMAL;
			e->abnormal.keycode = keycode;
			LOG_I("vpk.events.ABNORMAL = %d, %d %d", vpk.events.ABNORMAL, keycode, VPK_KEY_EVENT_NO_TF_CARD);
			break;
		// ALERT
		case VPK_KEY_EVENT_CAR_CRASH_WARNING:
		case VPK_KEY_EVENT_PARKING_CRASH_WARNING:
			e->type = vpk.events.ALERT;
			e->alert.keycode = keycode;
			LOG_I("vpk.events.ALERT = %d", vpk.events.ALERT);
			break;
		// NOTICE
		case VPK_KEY_EVENT_SYS_ACC_ON:
		case VPK_KEY_EVENT_SYS_ACC_OFF:
		case VPK_KEY_EVENT_SYS_FAST_ACC:
		case VPK_KEY_EVENT_SYS_FAST_DEC:
		case VPK_KEY_EVENT_SYS_FAST_TURN:
		case VPK_KEY_EVENT_SYS_WAKE_UP_KEY:
		case VPK_KEY_EVENT_SYS_WAKE_UP_ACC:
		case VPK_KEY_EVENT_SYS_WAKE_UP_GSENSOR:
		case VPK_KEY_EVENT_SYS_WAKE_UP_4G:
		case VPK_KEY_EVENT_SYS_POWER_OFF_KEY:
		case VPK_KEY_EVENT_SYS_POWER_OFF_ACC:
		case VPK_KEY_EVENT_SYS_POWER_OFF_GSENSOR:
		case VPK_KEY_EVENT_SYS_POWER_OFF_TIMA:
		case VPK_KEY_EVENT_QRCODE_GEN:
		case VPK_KEY_EVENT_FIRMWARE_DOWNLOAD:
		case VPK_KEY_EVENT_UPDATE_SYSTEM:
		case VPK_KEY_EVENT_UPDATE_SYS_TIME:
		case VPK_KEY_EVENT_GET_CUR_VIDEO_SUCCESS:
		case VPK_KEY_EVENT_GET_CUR_VIDEO_FAILED:
			e->type = vpk.events.NOTICE;
			e->notice.keycode = keycode;
			LOG_I("vpk.events.NOTICE = %d", vpk.events.NOTICE);
			break;
		default:
			LOG_W("event keycode unrecognized, value = %d(0x%x)", keycode, keycode);
			break;
		}

		LOG_W("Alert message keycode(type:%x): 0x%x, string: %s, len: %d\n", e->type, keycode, thiz->recv_buff, len);
	}

	return len;
}

static int nt966x_eventq_post(vpk_eventq_t *queue, vpk_event_t* e)
{
	int ret = 0;
// 	struct timespec ts;
	vpk_eventq_t* thiz = queue;
	char send_buff[EVENT_MQ_MSG_LEN_MAX];
	return_val_if_fail(thiz != NULL && send_buff != NULL && e != NULL, -1);

	switch (e->type)
	{
	case VPK_EVENT_ALERT:	//vpk.events.ALERT:
		snprintf(send_buff, sizeof(send_buff), "%d", e->alert.keycode);
		break;
	case VPK_EVENT_ABNORMAL:
		snprintf(send_buff, sizeof(send_buff), "%d", e->abnormal.keycode);
		break;
	case VPK_EVENT_NOTICE:
		snprintf(send_buff, sizeof(send_buff), "%d", e->notice.keycode);
		break;
	default:break;
	}

	LOG_I("event post msg type: %d, keycode: 0x%x, string: %s", e->type, e->alert.keycode, send_buff);

// 	ts.tv_sec	= 3;
// 	ts.tv_nsec	= 0;
// 	ret = mq_timedsend(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0, &ts);
	ret = mq_send(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0);
	if (ret != 0)
	{
		LOG_E("mq_timedsend error(name = %s), ret = %d, fd: %d", thiz->name, ret, thiz->fd);
		return -1;
	}

	return 0;
}

static int nt966x_eventq_close(vpk_eventq_t *queue)
{
	vpk_eventq_t* thiz = queue;

	if (thiz && thiz->fd >= 0)
	{
		LOG_I("event queue \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		mq_close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("eventq(fd) has been closed!");
	}

	return 0;
}

static const eventq_ops nt966x_eventq_ops = 
{
	nt966x_eventq_init,
	nt966x_eventq_empty,
	nt966x_eventq_recv,
	nt966x_eventq_post,
	nt966x_eventq_close,
};

const eventq_ops* get_nt966x_eventq_ops(void)
{
	return &nt966x_eventq_ops;
}
