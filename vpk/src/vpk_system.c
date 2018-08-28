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
#elif defined(_GENERIC_MIPS_)
extern system_kset generic_system_kset;
#elif defined(_GENERIC_ARM_)
extern system_kset generic_system_kset;
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
#elif defined(_GENERIC_MIPS_)
	&generic_system_kset,
#elif defined(_GENERIC_ARM_)
	&generic_system_kset,
#else
	&x86_system_kset,
#endif

	NULL,
};

const char* archs[] = {"X86", "MIPS", "ARM", "Other"};
const char* vpk_chip_architecture(void) {
	const char* arch = archs[3];

#if defined(_X86_)
	arch = archs[0];
#elif defined(_NT966X_)
	arch = archs[1];
#elif defined(_HI35XX_)
	arch = archs[2];
#elif defined(_GENERIC_MIPS_)
	arch = archs[1];
#elif defined(_GENERIC_ARM_)
	arch = archs[2];
#else
	arch = archs[3];
#endif

	return arch;
}

static vpk_system_t* sys = NULL;

void init_subsystem(int argc, char* argv[])
{
	vpk_logger_init();
	vpk_sessionsys_init();
	vpk_eventq_init();
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
