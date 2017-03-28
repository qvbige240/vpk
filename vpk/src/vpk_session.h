/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_SESSION_H
#define VPK_SESSION_H

#include "vpk_typedef.h"
#include "vpk_object.h"

TIMA_BEGIN_DELS

struct vpk_sessionsys_t;
typedef struct vpk_sessionsys_t vpk_sessionsys_t;

typedef struct sessionsys_ops_s
{
	vpk_sessionsys_t* (*init)();
}sessionsys_ops;

struct vpk_sessionsys_t
{
	const sessionsys_ops* ssops;
};

/* a session struct, serial, telnet, ssh etc. */
struct vpk_session_t;
typedef struct vpk_session_t vpk_session_t;

typedef struct session_ops_s
{
	vpk_session_t*	(*load)(void *ctx);
	int				(*open)(vpk_session_t *session, void *data);
	int				(*read)(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit);
	int				(*write)(vpk_session_t *session, void *buf, size_t nbytes, int timout);
	int				(*close)(vpk_session_t *session);
	int				(*optset)(vpk_session_t *session);
}session_ops;

#define extends_session()		\
	extends_object();			\
	int fd;						\
	char name[16];				\
	const session_ops* s_ops

struct vpk_session_t
{
	extends_session();
};

int vpk_sessionsys_init(void);
vpk_sessionsys_t* vpk_sessionsys(void);

TIMA_END_DELS

#endif //VPK_SESSION_H
