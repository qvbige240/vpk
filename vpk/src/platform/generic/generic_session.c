/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "generic_session.h"
#include "generic_actuator.h"

static vpk_sessionsys_t session_sys = {0};
static const sessionsys_ops generic_sessionsys_ops;

static vpk_sessionsys_t* generic_sessionsys_init(void)
{
	vpk_sessionsys_t* thiz = &session_sys;
	if (thiz)
	{
		thiz->ssops = &generic_sessionsys_ops;
		//register_serial_device("serial", get_generic_serial_ops());
		register_actuator_device("actuator", get_generic_actuator_ops());
	}

	return thiz;
}

static const sessionsys_ops generic_sessionsys_ops = 
{
	generic_sessionsys_init,
};

const sessionsys_ops* get_generic_sessionsys_ops(void)
{
	return &generic_sessionsys_ops;
}
