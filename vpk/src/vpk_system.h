/*
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#ifndef VPK_SYSTEM_H
#define VPK_SYSTEM_H

#include "vpk_logger.h"
#include "vpk_session.h"
#include "vpk_ieventq.h"

TIMA_BEGIN_DELS

typedef struct vpk_system_t vpk_system_t;

typedef struct system_kset
{
	vpk_system_t*			(*init)(void);
	const logger_ops*		(*get_logger_ops)(void);
	const sessionsys_ops*	(*get_sessionsys_ops)(void);
	const eventq_ops*		(*get_eventq_ops)(void);
}system_kset;

struct vpk_system_t
{
	const system_kset* sys_kset;
};

vpk_system_t* vpk_system(void);
int vpk_system_init(int argc, char* argv[]);
void vpk_system_destruct(vpk_system_t* thiz);
const char* vpk_chip_architecture(void);

TIMA_END_DELS

#endif //VPK_SYSTEM_H
