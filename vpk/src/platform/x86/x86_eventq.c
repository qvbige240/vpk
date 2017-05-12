/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "x86_eventq.h"
#include "vpk_event.h"
#include "vpk_logging.h"
#include "vpk_util.h"

// //customed event from uITRON
// #define CUSTOM_EVT_PREFIX				"CST,"
// #define CUSTOM_EVT_MQ_MSG_NUM_MAX		10
// #define CUSTOM_EVT_MQ_MSG_LEN_MAX		12
// #define CUSTOM_NOTIFY_EVENT_QUEUE_NAME	"/NvtCustomEventMq"

#include "vpk_constants.h"

#define MQUEUE_BUFFER_SIZE		EVENT_MQ_MSG_LEN_MAX

typedef struct queue_message
{
	long msg_type;
	char msg_value[MQUEUE_BUFFER_SIZE];
}queue_message_t;

const vpk_constants_t vpk = {
	{
		VPK_EVENT_ABNORMAL,
		VPK_EVENT_ALERT,
		VPK_EVENT_NOTICE,
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
		// NOTICE
		VPK_KEY_EVENT_QRCODE_GEN,
		VPK_KEY_EVENT_FIRMWARE_DOWNLOAD,
		VPK_KEY_EVENT_UPDATE_SYSTEM,
	},
};


static int msg_queue_init(char* fname, int id)
{
	int qid;
	key_t key;

	//if ((key = ftok(".", 'a')) == -1)
	if ((key = ftok(fname, id)) == -1)
	{
		LOG_E("ftok error!\n");
		return -1;
	}

	if ((qid = msgget(key, IPC_CREAT|0666)) == -1)
	{
		LOG_E("msgget error!\n");
		return -1;
	}

	LOG_I("queue %d opened\n", qid);

	return qid;
}

static int msg_queue_is_empty(int qid)
{
	int ret = 0;
	struct msqid_ds buf;

	if ((msgctl(qid, IPC_STAT, &buf)) != 0)
	{
		LOG_E("msgctl error");
	}

	if (buf.msg_qnum != 0)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}

	return ret;
}

static int msg_queue_post(int qid, void* msg_value)
{
	queue_message_t message = {0};

	message.msg_type = getpid();
	memcpy(message.msg_value, msg_value, sizeof(message.msg_value));

	if (msgsnd(qid, &message, MQUEUE_BUFFER_SIZE, 0) < 0)
	{
		LOG_E("msgsnd ERROR!\n");
		return -1;
	}

	return 0;
}

static int msg_queue_recv(int qid, void* msg_value)
{
	queue_message_t message = {0};

	if (msgrcv(qid, (void*)&message, MQUEUE_BUFFER_SIZE, 0, 0) < 0)
	{
		LOG_E("msgrcv ERROR!\n");
		return -1;
	}
	memcpy(msg_value, (void*)&message.msg_value, MQUEUE_BUFFER_SIZE);

	return 0;
}


static int x86_eventq_destruct(void *queue)
{
	vpk_eventq_t* thiz = (vpk_eventq_t*)queue;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("eventq \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			//mq_close(thiz->fd);
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
	vpk_eventq_t* thiz = NULL;
	mode = mode;
	thiz = (vpk_eventq_t*)TIMA_MALLOC(sizeof(vpk_eventq_t));
	if (thiz)
	{
		memset(thiz, 0x00, sizeof(*thiz));
		thiz->destruct	= x86_eventq_destruct;
		thiz->fd		= -1;
		memcpy(thiz->name, name, sizeof(thiz->name));
		thiz->fd = msg_queue_init(".", 'a');
		if (thiz->fd < 0)
		{
			LOG_E("mq_open error(name = %s), queue id: %d", thiz->name, thiz->fd);
			TIMA_FREE(thiz);
			return NULL;
		}
		LOG_D("eventq(name = \'%s\') opened, queue id = %d", thiz->name, thiz->fd);
	}

	return thiz;
}

static int x86_eventq_empty(const vpk_eventq_t* queue)
{
	return msg_queue_is_empty(queue->fd);
}


#define HEXSTR_PARSE(ptr)	\
	((vpk_hex_to_int(*(ptr)) << 12) | (vpk_hex_to_int(*((ptr)+1)) << 8) \
	| (vpk_hex_to_int(*((ptr)+2)) << 4) | vpk_hex_to_int(*((ptr)+3)))

static int x86_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e)
{
	int ret = 0, keycode = 0;
	vpk_eventq_t* thiz = queue;
	return_val_if_fail(thiz != NULL && e != NULL, -1);

	memset(thiz->recv_buff, 0x00, sizeof(thiz->recv_buff));
	ret = msg_queue_recv(thiz->fd, thiz->recv_buff);
	if (ret < 0) 
	{
		LOG_E("msg_queue_recv error(name = %s), ret = %d, qid: %d", thiz->name, thiz->fd);
		return -1;
	}

	//unsigned int keycode = atoi(thiz->recv_buff);
	keycode = HEXSTR_PARSE(&thiz->recv_buff[0]);
// 	int keycode = 0;
// 	sprintf(&keycode, "%x", thiz->recv_buff);
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
		LOG_I("vpk.events.ABNORMAL = %d, %d %d", vpk.events.ABNORMAL, keycode, VPK_KEY_EVENT_NO_TF_CARD);
		break;
	// ALERT
	case VPK_KEY_EVENT_CAR_CRASH_WARNING:
	case VPK_KEY_EVENT_PARKING_CRASH_WARNING:
		e->type = vpk.events.ALERT;
		e->alert.keycode = keycode;
		LOG_I("vpk.events.ALERT = %d", vpk.events.ALERT);
		break;
	case VPK_KEY_EVENT_QRCODE_GEN:
	case VPK_KEY_EVENT_FIRMWARE_DOWNLOAD:
	case VPK_KEY_EVENT_UPDATE_SYSTEM:
		e->type = vpk.events.NOTICE;
		e->notice.keycode = keycode;
		LOG_I("vpk.events.NOTICE = %d", vpk.events.NOTICE);
		break;
	default:
		LOG_W("vpk.events.other = %d, %d", vpk.events.ABNORMAL, keycode);
		break;
	}

	LOG_W("Alert message keycode(type:%x): 0x%x, string: %s\n", e->type, keycode, thiz->recv_buff);

	return ret;
}

static int x86_eventq_post(vpk_eventq_t *queue, vpk_event_t* e)
{
	int ret = 0;
//	struct timespec ts;
	vpk_eventq_t* thiz = queue;
	char send_buff[EVENT_MQ_MSG_LEN_MAX];
	return_val_if_fail(thiz != NULL && send_buff != NULL && e != NULL, -1);

	switch (e->type)
	{
	case VPK_EVENT_ALERT:	//vpk.events.ALERT:
		snprintf(send_buff, sizeof(send_buff), "%x", e->alert.keycode);
		break;
	case VPK_EVENT_ABNORMAL:
		snprintf(send_buff, sizeof(send_buff), "%x", e->abnormal.keycode);
		break;
	case VPK_EVENT_NOTICE:
		snprintf(send_buff, sizeof(send_buff), "%x", e->notice.keycode);
		break;
	default:break;
	}

	LOG_I("event post msg type: %d, keycode: 0x%x, string: %s", e->type, e->alert.keycode, send_buff);

// 	ts.tv_sec	= 3;
// 	ts.tv_nsec	= 0;
// 	ret = mq_timedsend(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0, &ts);
//	ret = mq_send(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0);
	ret = msg_queue_post(thiz->fd, send_buff);
	if (ret != 0)
	{
		LOG_E("mq_timedsend error(name = %s), ret = %d, qid: %d", thiz->name, ret, thiz->fd);
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
		//mq_close(thiz->fd);
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
