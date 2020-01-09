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

#include "generic_eventq.h"
#include "vpk_event.h"
#include "vpk_logging.h"
#include "vpk_util.h"
#include "vpk_constants.h"

#include "jansson.h"


const vpk_keymap_t key_event_map[] =
{
	//{ VPK_KEY_EVENT_SYS_ACC_ON, VPK_EVENT_NOTICE, 0x1001, "acc on" },

#define KEY_EVENT(NAME, TYPE, FIELD, VALUE, READABLE)	{ NAME, TYPE, VALUE, READABLE },
#include "macro_event.h"
#undef KEY_EVENT
};

const vpk_constants_t vpk = {
	{
		VPK_EVENT_EXCEP,
		VPK_EVENT_ALERT,
		VPK_EVENT_NOTICE,
	},
	{
		#define KEY_EVENT(NAME, TYPE, FIELD, VALUE, READABLE)	VALUE,
		#include "macro_event.h"
		// KEY_EVENT(SYS_ACC_ON, VPK_EVENT_NOTICE, EVENT_SYS_ACC_ON, 0x1001, "acc on")
		#undef KEY_EVENT
		/*
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
		VPK_KEY_EVENT_SYS_POWER_OFF_VOLTAGE_LOW,
		// NOTICE
		VPK_KEY_EVENT_QRCODE_GEN,
		VPK_KEY_EVENT_FIRMWARE_DOWNLOAD,
		VPK_KEY_EVENT_UPDATE_SYSTEM,
		VPK_KEY_EVENT_DEVICE_UNBIND,

		VPK_KEY_EVENT_GET_CUR_VIDEO_SUCCESS,
		VPK_KEY_EVENT_GET_CUR_VIDEO_FAILED,

		VPK_KEY_EVENT_CAR_CRASH_NOTICE,
		VPK_KEY_EVENT_PARKING_CRASH_NOTICE,	
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
		//VPK_KEY_EVENT_POLICE_COME_IN,
		//VPK_KEY_EVENT_CAR_FAST_ACCEL,
		//VPK_KEY_EVENT_CAR_FAST_SLOW_DOWN,
		//VPK_KEY_EVENT_CAR_RAPID_TURN,
		// NOTICE
		VPK_KEY_EVENT_UPDATE_SYS_TIME,
		*/
	},
};

#ifdef TIMA_PROTOCOL
#include "timapi/tima_eventq.h"
#define MQUEUE_BUFFER_SIZE		EVENT_QUEUE_MSG_LEN_MAX
#define MQUEUE_EVENT_PATH		"/tmp/tima.ipc"
#else

#define MQUEUE_BUFFER_SIZE		512
#define MQUEUE_EVENT_PATH		"/tmp/tima.ipc"

typedef struct queue_message
{
	long msg_type;
	char msg_value[MQUEUE_BUFFER_SIZE];
}queue_message_t;

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

#define tima_mqueue_open()		msg_queue_init(".", 'a')
#define tima_mqueue_is_empty	msg_queue_is_empty
#define tima_mqueue_recv		msg_queue_recv
#define tima_mqueue_post		msg_queue_post

#endif // TIMA_PROTOCOL

static int generic_eventq_destruct(void *queue)
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

static vpk_eventq_t* generic_eventq_init(const char* name, const char* mode)
{
	vpk_eventq_t* thiz = NULL;
	mode = mode;
	thiz = (vpk_eventq_t*)TIMA_MALLOC(sizeof(vpk_eventq_t));
	if (thiz)
	{
		memset(thiz, 0x00, sizeof(*thiz));
		thiz->destruct	= generic_eventq_destruct;
		thiz->fd		= -1;
		memcpy(thiz->name, name, sizeof(thiz->name));
		//thiz->fd = msg_queue_init(".", 'a');
		thiz->fd = tima_mqueue_open();
		if (thiz->fd < 0)
		{
			LOG_E("[name = %s]tima_mqueue_open error, queue id: %d", thiz->name, thiz->fd);
			TIMA_FREE(thiz);
			return NULL;
		}
		LOG_D("eventq(name = \'%s\') opened, queue id = %d", thiz->name, thiz->fd);
	}

	return thiz;
}

static int generic_eventq_empty(const vpk_eventq_t* queue)
{
	//return msg_queue_is_empty(queue->fd);
	return tima_mqueue_is_empty(queue->fd);
}


#define HEXSTR_PARSE(ptr)	\
	((vpk_hex_to_int(*(ptr)) << 12) | (vpk_hex_to_int(*((ptr)+1)) << 8) \
	| (vpk_hex_to_int(*((ptr)+2)) << 4) | vpk_hex_to_int(*((ptr)+3)))

static int event_code_get(char* data)
{
	int ret = -1;
	json_t* json_root = json_loads(data, 0, NULL);
	if (json_root) {

		json_t *jobject = json_object_get(json_root, "event");
		if (!jobject) {
			LOG_E("jobject[event] not exist, event data format error.");
		} else {
			char* event = (char*)json_string_value(jobject);
			if (event)
				ret = HEXSTR_PARSE(event);
		}

		json_decref(json_root);
	}

	if (ret == -1)
		LOG_E("[json]event data format error.\n");
	return ret;
}

static int generic_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e)
{
	int ret = 0, keycode = 0;
	vpk_eventq_t* thiz = queue;
	return_val_if_fail(thiz != NULL && e != NULL, -1);

	memset(thiz->recv_buff, 0x00, sizeof(thiz->recv_buff));
	//ret = msg_queue_recv(thiz->fd, thiz->recv_buff);
	ret = tima_mqueue_recv(thiz->fd, thiz->recv_buff);
	if (ret < 0) 
	{
		LOG_E("tima_mqueue_recv error(name = %s), ret = %d, qid: %d", thiz->name, thiz->fd);
		return -1;
	}

	//unsigned int keycode = atoi(thiz->recv_buff);
	//keycode = HEXSTR_PARSE(&thiz->recv_buff[0]);
	keycode = event_code_get(&thiz->recv_buff[0]);
// 	int keycode = 0;
// 	sprintf(&keycode, "%x", thiz->recv_buff);
#if 1

	int i;
	for (i = 0; i < VPK_KEY_EVENT_MAX_NUM; i++) {
		if (key_event_map[i].value == keycode) {

			e->type = key_event_map[i].type;
			//e->notice.keycode = key_event_map[i].key;
			e->notice.keycode = keycode;
			e->notice.data = thiz->recv_buff;
			LOG_I("[0x%x]vpk.events (key, value)(%d, 0x%04x)", e->type, key_event_map[i].key, key_event_map[i].value);
			break;
		}
	}
	if (i == VPK_KEY_EVENT_MAX_NUM) {
		if (keycode != 0x9999)
			LOG_W("event keycode unrecognized, value = %d(0x%x)", keycode, keycode);
	}

#else
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
	case VPK_KEY_EVENT_SYS_POWER_OFF_VOLTAGE_LOW:
	case VPK_KEY_EVENT_QRCODE_GEN:
	case VPK_KEY_EVENT_FIRMWARE_DOWNLOAD:
	case VPK_KEY_EVENT_UPDATE_SYSTEM:
	case VPK_KEY_EVENT_DEVICE_UNBIND:
	case VPK_KEY_EVENT_UPDATE_SYS_TIME:
	case VPK_KEY_EVENT_GET_CUR_VIDEO_SUCCESS:
	case VPK_KEY_EVENT_GET_CUR_VIDEO_FAILED:
	case VPK_KEY_EVENT_CAR_CRASH_NOTICE:
	case VPK_KEY_EVENT_PARKING_CRASH_NOTICE:
		e->type = vpk.events.NOTICE;
		e->notice.keycode = keycode;
		LOG_I("vpk.events.NOTICE = %d", vpk.events.NOTICE);
		break;
	default:
		LOG_W("vpk.events.other = %d, %d", vpk.events.ABNORMAL, keycode);
		break;
	}
#endif
	LOG_W("Alert message keycode(type:%x): 0x%x, string: %s\n", e->type, keycode, thiz->recv_buff);

	return ret;
}

static int generic_eventq_post(vpk_eventq_t *queue, vpk_event_t* e)
{
	int ret = 0;
//	struct timespec ts;
	vpk_eventq_t* thiz = queue;
	char send_buff[MQUEUE_BUFFER_SIZE] = {0};
	char send_code[16] = {0};
	return_val_if_fail(thiz != NULL && send_code != NULL && e != NULL, -1);

	switch (e->type)
	{
	case VPK_EVENT_ALERT:	//vpk.events.ALERT:
		snprintf(send_code, sizeof(send_code), "%04x", e->alert.keycode);
		break;
	case VPK_EVENT_EXCEP:
		snprintf(send_code, sizeof(send_code), "%04x", e->abnormal.keycode);
		break;
	case VPK_EVENT_NOTICE:
		snprintf(send_code, sizeof(send_code), "%04x", e->notice.keycode);
		break;
	default:
		LOG_D("post default type, keycode: %04x", e->alert.keycode);
		snprintf(send_code, sizeof(send_code), "%04x", e->alert.keycode);
		break;
	}

	LOG_I("event post msg type: %d, keycode: 0x%x, string: %s", e->type, e->alert.keycode, send_code);

// 	ts.tv_sec	= 3;
// 	ts.tv_nsec	= 0;
// 	ret = mq_timedsend(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0, &ts);
//	ret = mq_send(thiz->fd, send_buff, EVENT_MQ_MSG_LEN_MAX, 0);
	//ret = msg_queue_post(thiz->fd, send_buff);
	ret = tima_mqueue_post(thiz->fd, send_buff);
	if (ret != 0)
	{
		LOG_E("mq_timedsend error(name = %s), ret = %d, qid: %d", thiz->name, ret, thiz->fd);
		return -1;
	}

	return 0;
}

static int generic_eventq_close(vpk_eventq_t *queue)
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

static const eventq_ops generic_eventq_ops = 
{
	generic_eventq_init,
	generic_eventq_empty,
	generic_eventq_recv,
	generic_eventq_post,
	generic_eventq_close,
};

const eventq_ops* get_generic_eventq_ops(void)
{
	return &generic_eventq_ops;
}