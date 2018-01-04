/**
 * History:
 * ================================================================
 * 2018-01-03 qing.zou created
 *
 */
#include "vpk_logging.h"

#include "vpk_cdevice.h"
#include "vpk_actuator.h"

static cdevice_register_t container = {0};

int register_actuator_device(const char* standard, const session_ops* sops)
{
	return_val_if_fail(standard != NULL && sops != NULL, -1);

	return vpk_register_cdevice(&container, standard, sops, "session");
}

static vpk_actuator_t* vpk_actuator_load(void *ctx)
{
	vpk_actuator_t* actuator = NULL;
	const session_ops *s_ops = NULL;

	s_ops = (const session_ops *)vpk_find_cdevice(&container, "actuator");
	if (s_ops)
	{
		actuator = (vpk_actuator_t*)s_ops->load((void*)ctx);
		return_val_if_fail(actuator != NULL, NULL);
		actuator->s_ops = s_ops;
		actuator->data_buff = NULL;
		actuator->total_size = 0;
		memset(actuator->name, 0, _countof(actuator->name));
		//strlcpy(actuator->name, ctx, _countof(actuator->name));
		memcpy(actuator->name, ctx, sizeof(actuator->name));
		LOG_D("actuator->name: %s, type: %s", actuator->name, ctx);
	}

	return actuator;
}

VPKAPI vpk_actuator_t* vpk_actuator_create(void *name)
{
	vpk_actuator_t* actuator = vpk_actuator_load(name);
	return_val_if_fail(actuator != NULL, NULL);

	return actuator;
}

VPKAPI int vpk_actuator_open(vpk_actuator_t* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	return thiz->s_ops->open((vpk_session_t *)thiz, thiz->name);
}

/*VPKAPI int vpk_actuator_read(vpk_actuator_t* thiz, void *buf, size_t nbytes, int timout, SerialVisitFunc visit)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL && buf != NULL, -1);

	if (nbytes > 0)
	{
		return thiz->s_ops->read((vpk_session_t *)thiz, buf, nbytes, timout, (void*)visit);
	}

	return 0;
}*/

VPKAPI int vpk_actuator_write(vpk_actuator_t* thiz, void* buf, ssize_t nbytes, 
							  void* recv_buff, ssize_t size_buff, int timout)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL && recv_buff != NULL, -1);

	thiz->data_buff  = recv_buff;
	thiz->total_size = size_buff;
	if (size_buff < 1)
		LOG_W("receive buffer size is not compatible.");

	return thiz->s_ops->write((vpk_session_t *)thiz, buf, nbytes, timout);
}

VPKAPI int vpk_actuator_close(vpk_actuator_t* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	thiz->data_buff = NULL;
	thiz->total_size = 0;
	return thiz->s_ops->close((vpk_session_t *)thiz);
}

VPKAPI int vpk_actuator_destroy(vpk_actuator_t* thiz)
{
	if (thiz && thiz->destruct)
	{
		return thiz->destruct(thiz);
	}

	return 0;
}
