/*
 * History:
 * ================================================================
 * 2017-03-27 qing.zou created
 *
 */
#include <unistd.h>

#include "vpk_logging.h"
#include "generic_actuator.h"

const ActionInfo vpk_action_tables[] =
{
	/** it needs order by 'VpkActionType' **/
	{VPK_ACTION_START_TAG,					"UCUSTOM",	"	"},
};
const int vpk_action_tables_size = _countof(vpk_action_tables);

static int generic_actuator_destruct(void *session)
{
	return 0;
}

static vpk_session_t* generic_actuator_load(void *ctx)
{
	vpk_actuator_t* thiz = NULL;
	ctx = ctx;

	return (vpk_session_t*)thiz;
}

static int generic_actuator_open(vpk_session_t *session, void *data)
{
	return -1;
}


static int generic_actuator_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	return 0;
}

static int generic_actuator_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int ret = -1;
	return ret;
}

static int generic_actuator_close(vpk_session_t *session)
{
	return 0;
}

static int generic_actuator_opt_set(vpk_session_t *session)
{
	return 0;
}

/* The actuator operations */
static const session_ops generic_actuator_ops =
{
	generic_actuator_load,
	generic_actuator_open,
	generic_actuator_read,
	generic_actuator_write,
	generic_actuator_close,
	generic_actuator_opt_set,
};

const session_ops* get_generic_actuator_ops(void)
{
	return (const session_ops*)&generic_actuator_ops;
}

