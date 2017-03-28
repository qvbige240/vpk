/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */

#include "vpk_logging.h"

#include "vpk_cdevice.h"
#include "vpk_nvtuctrl.h"

static cdevice_register_t container = {0};

int register_nvtuctrl_device(const char* standard, const session_ops* sops)
{
	return_val_if_fail(standard != NULL && sops != NULL, -1);

	return vpk_register_cdevice(&container, standard, sops, "session");
}

static vpk_nvtuctrl_t* vpk_nvtuctrl_load(void *ctx)
{
	vpk_nvtuctrl_t* nvtuctrl = NULL;
	const session_ops *s_ops = NULL;
	//return_val_if_fail(ctx != NULL, NULL);

	s_ops = (const session_ops *)vpk_find_cdevice(&container, "nvtuctrl");
	if (s_ops)
	{
		nvtuctrl = (vpk_nvtuctrl_t*)s_ops->load((void*)ctx);
		return_val_if_fail(nvtuctrl != NULL, NULL);
		nvtuctrl->s_ops = s_ops;
		nvtuctrl->data_buff = NULL;
		nvtuctrl->total_size = 0;
		memset(nvtuctrl->name, 0, _countof(nvtuctrl->name));
		strlcpy(nvtuctrl->name, ctx, _countof(nvtuctrl->name));
		LOG_D("nvtuctrl->name: %s, type: %s", nvtuctrl->name, ctx);
	}

	return nvtuctrl;
}

VPKAPI vpk_nvtuctrl_t* vpk_nvtuctrl_create(void *name)
{
	vpk_nvtuctrl_t* nvtuctrl = vpk_nvtuctrl_load(name);
	return_val_if_fail(nvtuctrl != NULL, NULL);

	return nvtuctrl;
}

VPKAPI int vpk_nvtuctrl_open(vpk_nvtuctrl_t* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	return thiz->s_ops->open((vpk_session_t *)thiz, thiz->name);
}

/*VPKAPI int vpk_nvtuctrl_read(vpk_nvtuctrl_t* thiz, void *buf, size_t nbytes, int timout, SerialVisitFunc visit)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL && buf != NULL, -1);

	if (nbytes > 0)
	{
		return thiz->s_ops->read((vpk_session_t *)thiz, buf, nbytes, timout, (void*)visit);
	}

	return 0;
}*/

VPKAPI int vpk_nvtuctrl_write(vpk_nvtuctrl_t* thiz, void* buf, ssize_t nbytes, 
							  void* recv_buff, ssize_t size_buff, int timout)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL && recv_buff != NULL, -1);

	thiz->data_buff  = recv_buff;
	thiz->total_size = size_buff;
	if (size_buff < 1)
		LOG_W("receive buffer size is not compatible.");

	return thiz->s_ops->write((vpk_session_t *)thiz, buf, nbytes, timout);
}

VPKAPI int vpk_nvtuctrl_close(vpk_nvtuctrl_t* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	thiz->data_buff = NULL;
	thiz->total_size = 0;
	return thiz->s_ops->close((vpk_session_t *)thiz);
}

VPKAPI int vpk_nvtuctrl_destroy(vpk_nvtuctrl_t* thiz)
{
	if (thiz && thiz->destruct)
	{
		return thiz->destruct(thiz);
	}

	return 0;
}
