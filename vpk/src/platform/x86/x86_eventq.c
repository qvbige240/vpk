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
		X86_EVENT_ALERT,
	},
	{
		X86_KEY_EVENT_NO_TF_CARD,
		X86_KEY_EVENT_TF_CARD_WRITE_PROTECT,
		X86_KEY_EVENT_TF_CARD_FULL,
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

static vpk_eventq_t* x86_eventq_init(void *ctx)
{
	vpk_eventq_t* thiz = NULL;
	struct mq_attr attr;
	attr.mq_flags	= O_NONBLOCK;
	attr.mq_maxmsg	= CUSTOM_EVT_MQ_MSG_NUM_MAX;
	attr.mq_msgsize = CUSTOM_EVT_MQ_MSG_LEN_MAX;
	attr.mq_curmsgs = 0;
	thiz = (vpk_eventq_t*)TIMA_MALLOC(sizeof(vpk_eventq_t));
	if (thiz)
	{
		memset(thiz, 0x00, sizeof(*thiz));
		thiz->destruct	= x86_eventq_destruct;
		thiz->fd		= -1;
		memcpy(thiz->name, CUSTOM_NOTIFY_EVENT_QUEUE_NAME, sizeof(thiz->name));
		thiz->fd = mq_open(CUSTOM_NOTIFY_EVENT_QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
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

// 		switch (atoi(thiz->recv))
// 		{
// 		case vpk.keys.EVENT_NO_TF_CARD:
// 			e->type = vpk.events.ALERT;
// 			break;
// 		default:break;
// 		}

		e->alert.type = vpk.events.ALERT;
		e->alert.keycode = atoi(thiz->recv_buff);
		LOG_W("Alert message keycode: %d, string: %s, len: %d\n", atoi(thiz->recv_buff), thiz->recv_buff, len);
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
	case X86_EVENT_ALERT:	//vpk.events.ALERT:
		snprintf(send_buff, sizeof(send_buff), "0x%x", e->alert.keycode);
		break;
	//case vpk.events. other:
	//	break;
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
