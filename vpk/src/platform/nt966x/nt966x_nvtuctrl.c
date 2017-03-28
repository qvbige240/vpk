/*
 * History:
 * ================================================================
 * 2017-03-27 qing.zou created
 *
 */
#include <unistd.h>

#include "nvtuctrl.h"

#include "vpk_logging.h"
#include "nt966x_nvtuctrl.h"


static int nt966x_nvtuctrl_destruct(void *session)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			NvtUctrl_Close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->s_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_session_t* nt966x_nvtuctrl_load(void *ctx)
{
	vpk_nvtuctrl_t* thiz = NULL;
	ctx = ctx;

	thiz = (vpk_nvtuctrl_t*)TIMA_MALLOC(sizeof(vpk_nvtuctrl_t));
	if (thiz)
	{
		thiz->destruct   = (vpk_destruct_func)nt966x_nvtuctrl_destruct;
		thiz->data_buff  = NULL;
		thiz->total_size = 0;
		thiz->fd		 = -1;
	}

	return (vpk_session_t*)thiz;
}

static int nt966x_nvtuctrl_open(vpk_session_t *session, void *data)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	return_val_if_fail(thiz != NULL, -1);

	thiz->fd = NvtUctrl_Open();
	if (thiz->fd < 0)
	{
		LOG_E("nvtuctrl \'%s\' communication open failed!", thiz->name);
		return thiz->fd;
	}

	return thiz->fd;
}


static int nt966x_nvtuctrl_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	return 0;
}

static int nt966x_nvtuctrl_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int ret = -1;
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0 && thiz->data_buff != NULL, -1);
	timout = timout;
	
	if (buf && nbytes > 0 && thiz->total_size > 0)
	{
		ret = NvtUctrl_CmdSnd(thiz->fd, buf, thiz->data_buff, thiz->total_size);
	}

	return ret;
}

static int nt966x_nvtuctrl_close(vpk_session_t *session)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	if (thiz && thiz->fd >= 0)
	{
		LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		NvtUctrl_Close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("nvtuctrl(fd) has been closed!");
	}

	return 0;
}

static int nt966x_nvtuctrl_opt_set(vpk_session_t *session)
{
	return 0;
}

/* The nvtuctrl operations */
static const session_ops nt966x_nvtuctrl_ops =
{
	nt966x_nvtuctrl_load,
	nt966x_nvtuctrl_open,
	nt966x_nvtuctrl_read,
	nt966x_nvtuctrl_write,
	nt966x_nvtuctrl_close,
	nt966x_nvtuctrl_opt_set,
};

const session_ops* get_nt966x_nvtuctrl_ops(void)
{
	return (const session_ops*)&nt966x_nvtuctrl_ops;
}

