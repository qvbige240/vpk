/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "x86_system.h"
#include "x86_log.h"
#include "x86_session.h"
#include "x86_eventq.h"

static vpk_system_t x86_system = {0};

static int x86_system_destruct(vpk_system_t* self)
{
	vpk_system_destruct(self);
	return 0;
}

static vpk_system_t* x86_system_init(void)
{
	vpk_system_t* thiz = &x86_system;

	if (thiz)
	{
		thiz->sys_kset = &x86_system_kset;
	}
	(void)x86_system_destruct;

	return thiz;
}

system_kset x86_system_kset = 
{
	x86_system_init,
	get_x86_logger_ops,
	get_x86_sessionsys_ops,
	get_x86_eventq_ops,
};
