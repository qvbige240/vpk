/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "x86_session.h"
#include "x86_serial.h"
#include "x86_nvtuctrl.h"

static vpk_sessionsys_t session_sys = {0};
static const sessionsys_ops x86_sessionsys_ops;

static vpk_sessionsys_t* x86_sessionsys_init(void)
{
	vpk_sessionsys_t* thiz = &session_sys;
	if (thiz)
	{
		thiz->ssops = &x86_sessionsys_ops;
		register_serial_device("serial", get_x86_serial_ops());
		register_nvtuctrl_device("nvtuctrl", get_x86_nvtuctrl_ops());
	}

	return thiz;
}

static const sessionsys_ops x86_sessionsys_ops = 
{
	x86_sessionsys_init,
};

const sessionsys_ops* get_x86_sessionsys_ops(void)
{
	return &x86_sessionsys_ops;
}
