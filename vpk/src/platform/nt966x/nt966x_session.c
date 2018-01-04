/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "nt966x_session.h"
#include "nt966x_serial.h"
#include "nt966x_nvtuctrl.h"

static vpk_sessionsys_t session_sys = {0};
static const sessionsys_ops nt966x_sessionsys_ops;

static vpk_sessionsys_t* nt966x_sessionsys_init(void)
{
	vpk_sessionsys_t* thiz = &session_sys;
	if (thiz)
	{
		thiz->ssops = &nt966x_sessionsys_ops;
		register_serial_device("serial", get_nt966x_serial_ops());
		register_actuator_device("actuator", get_nt966x_nvtuctrl_ops());
	}

	return thiz;
}

static const sessionsys_ops nt966x_sessionsys_ops = 
{
	nt966x_sessionsys_init,
};

const sessionsys_ops* get_nt966x_sessionsys_ops(void)
{
	return &nt966x_sessionsys_ops;
}
