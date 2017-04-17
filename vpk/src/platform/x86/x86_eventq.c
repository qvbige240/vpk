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

#include "x86_eventq.h"
#include "vpk_event.h"
#include "vpk_logging.h"

// //customed event from uITRON
#define CUSTOM_EVT_PREFIX				"CST,"
#define CUSTOM_EVT_MQ_MSG_NUM_MAX		10
#define CUSTOM_EVT_MQ_MSG_LEN_MAX		12
#define CUSTOM_NOTIFY_EVENT_QUEUE_NAME	"/NvtCustomEventMq"

#include "vpk_constants.h"

const vpk_constants_t vpk = {
	{
		VPK_EVENT_ABNORMAL,
		VPK_EVENT_ALERT,
	},
	{
		// ABNORMAL
		VPK_KEY_EVENT_NO_TF_CARD,
		VPK_KEY_EVENT_TF_CARD_WRITE_PROTECT,
		VPK_KEY_EVENT_TF_CARD_FULL,
		VPK_KEY_EVENT_FILE_READ_ERROR,
		VPK_KEY_EVENT_MEMORY_ERROR,
		VPK_KEY_EVENT_TF_CARD_WR_ERROR,
		VPK_KEY_EVENT_TF_CARD_MIN_ERROR,
		VPK_KEY_EVENT_REAR_SENSOR_REMOVE,
		VPK_KEY_EVENT_FILE_NOT_EXIST,
		// ALERT
		VPK_KEY_EVENT_CAR_CRASH_WARNING,
		// ALERT
		VPK_KEY_EVENT_PARKING_CRASH_WARNING,
		VPK_KEY_EVENT_POLICE_COME_IN,
		VPK_KEY_EVENT_CAR_FAST_ACCEL,
		VPK_KEY_EVENT_CAR_FAST_SLOW_DOWN,
		VPK_KEY_EVENT_CAR_RAPID_TURN,
	},
};

static int x86_eventq_destruct(void *queue)
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

static vpk_eventq_t* x86_eventq_init(const char* name, const char* mode)
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
		thiz->destruct	= x86_eventq_destruct;
		thiz->fd		= -1;
		memcpy(thiz->name, CUSTOM_NOTIFY_EVENT_QUEUE_NAME, sizeof(thiz->name));
		if (create)
			thiz->fd = mq_open(CUSTOM_NOTIFY_EVENT_QUEUE_NAME, oflags, 0777, &attr);
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

static int x86_eventq_empty(const vpk_eventq_t* queue)
{
	return 0;
}

static int x86_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e)
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

		int keycode = atoi(thiz->recv_buff);
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
		case VPK_KEY_EVENT_POLICE_COME_IN:
		case VPK_KEY_EVENT_CAR_FAST_ACCEL:
		case VPK_KEY_EVENT_CAR_FAST_SLOW_DOWN:
		case VPK_KEY_EVENT_CAR_RAPID_TURN:
			e->type = vpk.events.ABNORMAL;
			e->abnormal.keycode = keycode;
			break;
			// ALERT
		case VPK_KEY_EVENT_CAR_CRASH_WARNING:
		case VPK_KEY_EVENT_PARKING_CRASH_WARNING:
			e->type = vpk.events.ALERT;
			e->alert.keycode = keycode;
			break;
		default:break;
		}

		LOG_W("Alert message keycode(type:%x): 0x%x, string: %s, len: %d\n", e->type, keycode, thiz->recv_buff, len);
	}

	return len;
}

static int x86_eventq_post(vpk_eventq_t *queue, vpk_event_t* e)
{
	int ret = 0;
	struct timespec ts;
	vpk_eventq_t* thiz = queue;
	char send_buff[EVENT_MQ_MSG_LEN_MAX];
	return_val_if_fail(thiz != NULL && send_buff != NULL && e != NULL, -1);

	switch (e->type)
	{
	case VPK_EVENT_ALERT:	//vpk.events.ALERT:
		snprintf(send_buff, sizeof(send_buff), "0x%x", e->alert.keycode);
		break;
	case VPK_EVENT_ABNORMAL:
		snprintf(send_buff, sizeof(send_buff), "0x%x", e->abnormal.keycode);
		break;
	default:break;
	}

	LOG_I("event post msg type: %d, keycode: 0x%x, string: %s", e->type, e->alert.keycode, send_buff);

	ts.tv_sec	= 3;
	ts.tv_nsec	= 0;
	//ret = mq_timedsend(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0, &ts);
	ret = mq_send(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0);
	if (ret != 0)
	{
		LOG_E("mq_timedsend error(name = %s), ret = %d, fd: %d", thiz->name, ret, thiz->fd);
		return -1;
	}

	return 0;
}

static int x86_eventq_close(vpk_eventq_t *queue)
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

static const eventq_ops x86_eventq_ops = 
{
	x86_eventq_init,
	x86_eventq_empty,
	x86_eventq_recv,
	x86_eventq_post,
	x86_eventq_close,
};

const eventq_ops* get_x86_eventq_ops(void)
{
	return &x86_eventq_ops;
}
