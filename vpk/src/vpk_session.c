/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */

#include "vpk_system.h"
#include "vpk_session.h"

static vpk_sessionsys_t* sessionsys = NULL;

int vpk_sessionsys_init(void)
{
	const sessionsys_ops* ssops = NULL;
	vpk_system_t* sys = vpk_system();
	return_val_if_fail(sys != NULL, -1);

	ssops = sys->sys_kset->get_sessionsys_ops();
	return_val_if_fail(ssops != NULL, -1);

	sessionsys = ssops->init();
	return_val_if_fail(sessionsys != NULL, -1);

	return 0;
}

vpk_sessionsys_t* vpk_sessionsys(void)
{
	return sessionsys;
}
