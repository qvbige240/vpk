/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "nt966x_system.h"
#include "nt966x_log.h"
#include "nt966x_session.h"

static vpk_system_t nt966x_system = {0};

static int nt966x_system_destruct(vpk_system_t* self)
{
	vpk_system_destruct(self);
	return 0;
}

static vpk_system_t* nt966x_system_init(void)
{
	vpk_system_t* thiz = &nt966x_system;

	if (thiz)
	{
		thiz->sys_kset = &nt966x_system_kset;
	}
	(void)nt966x_system_destruct;

	return thiz;
}

system_kset nt966x_system_kset = 
{
	nt966x_system_init,
	get_nt966x_logger_ops,
	get_nt966x_sessionsys_ops,
};
