/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "x86_session.h"
#include "x86_serial.h"
//#include "x86_nvtuctrl.h"

static vpk_sessionsys_t session_sys = {0};
static const sessionsys_ops x86_sessionsys_ops;

static vpk_sessionsys_t* x86_sessionsys_init(void)
{
	vpk_sessionsys_t* thiz = &session_sys;
	if (thiz)
	{
		thiz->ssops = &x86_sessionsys_ops;
		register_serial_device("serial", get_x86_serial_ops());
		//register_actuator_device("nvtuctrl", get_x86_nvtuctrl_ops());	// for mock _NT966X_
#ifdef TIMA_PROTOCOL
	#include "hi35xx_actuator.h"
		register_actuator_device("actuator", get_hi35xx_actuator_ops());
#else
	#include "x86_actuator.h"
		register_actuator_device("actuator", get_x86_actuator_ops());
#endif // TIMA_PROTOCOL
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
