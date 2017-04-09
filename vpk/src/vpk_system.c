/*
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */

#include "vpk_system.h"

//#define _NT966X_

#if defined(_X86_)
extern system_kset x86_system_kset;
#elif defined(_HI35XX_)
//extern system_kset hi35xx_system_kset;
#elif defined(_NT966X_)
extern system_kset nt966x_system_kset;
#else
extern system_kset x86_system_kset;
#endif

const system_kset* const systems[] = 
{

#if defined(_X86_)
	&x86_system_kset,
#elif defined(_NT966X_)
	&nt966x_system_kset,
#elif defined(_HI35XX_)
//	&hi35xx_system_kset,
#else
	&x86_system_kset,
#endif

	NULL,
};

static vpk_system_t* sys = NULL;

void init_subsystem(int argc, char* argv[])
{
	vpk_logger_init();
	vpk_sessionsys_init();
}

int vpk_system_init(int argc, char* argv[])
{
	if (systems[0] != NULL)
		sys = systems[0]->init();
	return_val_if_fail(sys != NULL, -1);

 	sys->sys_kset = systems[0];
	init_subsystem(argc, argv);

	return 0;
}

vpk_system_t* vpk_system(void)
{
	return sys;
}

void vpk_system_destruct(vpk_system_t* thiz)
{
	thiz = thiz;
}
