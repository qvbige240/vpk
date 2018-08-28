/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "generic_system.h"
#include "generic_log.h"
#include "generic_session.h"
#include "generic_eventq.h"

static vpk_system_t generic_system = {0};

static int generic_system_destruct(vpk_system_t* self)
{
	vpk_system_destruct(self);
	return 0;
}

static vpk_system_t* generic_system_init(void)
{
	vpk_system_t* thiz = &generic_system;

	if (thiz)
	{
		thiz->sys_kset = &generic_system_kset;
	}
	(void)generic_system_destruct;

	return thiz;
}

system_kset generic_system_kset = 
{
	generic_system_init,
	get_generic_logger_ops,
	get_generic_sessionsys_ops,
	get_generic_eventq_ops,
};
