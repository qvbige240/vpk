/*
 * History:
 * ================================================================
 * 2017-03-27 qing.zou created
 *
 */
#include <unistd.h>

//#include "nvtuctrl.h"

#include "vpk_logging.h"
#include "x86_nvtuctrl.h"


static int x86_nvtuctrl_destruct(void *session)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			//NvtUctrl_Close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->s_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_session_t* x86_nvtuctrl_load(void *ctx)
{
	vpk_nvtuctrl_t* thiz = NULL;
	ctx = ctx;

	thiz = (vpk_nvtuctrl_t*)TIMA_MALLOC(sizeof(vpk_nvtuctrl_t));
	if (thiz)
	{
		thiz->destruct   = (vpk_destruct_func)x86_nvtuctrl_destruct;
		thiz->data_buff  = NULL;
		thiz->total_size = 0;
		thiz->fd		 = -1;
	}

	return (vpk_session_t*)thiz;
}

static int x86_nvtuctrl_open(vpk_session_t *session, void *data)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	return_val_if_fail(thiz != NULL, -1);

	//thiz->fd = NvtUctrl_Open();
	thiz->fd = 1;
	if (thiz->fd < 0)
	{
		LOG_E("nvtuctrl \'%s\' communication open failed!", thiz->name);
		return thiz->fd;
	}

	return thiz->fd;
}


static int x86_nvtuctrl_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	return 0;
}

static int x86_nvtuctrl_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int ret = -1;
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0 && thiz->data_buff != NULL, -1);
	timout = timout;
	
	// fill the receive buffer with test data
	if (buf && nbytes > 0 && thiz->total_size > 0)
	{
		//ret = NvtUctrl_CmdSnd(thiz->fd, buf, thiz->data_buff, thiz->total_size);
		char *p = NULL;
		//GPS
		p = strstr(buf, "-gpsinfo");
		if (p != NULL) 
		{
            char *str = "{\"Latitude\":30.657300,\"Longitude\":104.065855,\"Speed\":0.300003,\"Angle\":0.400004}";
			//char *str = "{\"Latitude\":0.100001,\"Longitude\":0.200002,\"Speed\":0.300003,\"Angle\":0.400004}";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		// SNAP
		p = strstr(buf, "-snapshot");
		if (p != NULL)
		{
			char *str = "resources/test_2017_0409_171946_RE.JPG";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		// VIDEO
		p = strstr(buf, "-movierec");
		if (p != NULL)
		{
			char *str = "resources/test_2017.mp4";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-movielen");
		if (p != NULL)
		{
			char *str = "movie len:8288";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "get");
		if (p != NULL)
		{
			char *str = "0";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		else
		{
			char *str = "unkonwn command!!!";
			memcpy(thiz->data_buff, str, strlen(str));
		}
		
	}

	return ret;
}

static int x86_nvtuctrl_close(vpk_session_t *session)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	if (thiz && thiz->fd >= 0)
	{
		LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		//NvtUctrl_Close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("nvtuctrl(fd) has been closed!");
	}

	return 0;
}

static int x86_nvtuctrl_opt_set(vpk_session_t *session)
{
	return 0;
}

/* The nvtuctrl operations */
static const session_ops x86_nvtuctrl_ops =
{
	x86_nvtuctrl_load,
	x86_nvtuctrl_open,
	x86_nvtuctrl_read,
	x86_nvtuctrl_write,
	x86_nvtuctrl_close,
	x86_nvtuctrl_opt_set,
};

const session_ops* get_x86_nvtuctrl_ops(void)
{
	return (const session_ops*)&x86_nvtuctrl_ops;
}

