/*
 * History:
 * ================================================================
 * 2018-01-04 qing.zou created
 *
 */
#include <unistd.h>

#include "timapi/tima_rpc.h"

#include "vpk_logging.h"
#include "hi35xx_actuator.h"

const ActionInfo vpk_action_tables[] =
{
	/** it needs order by 'VpkActionType' **/
	{VPK_ACTION_START_TAG,					"ACTIONS",	"	"},
	{VPK_ACTION_GPSINFO,					"GPS",		"tima -gpsinfo "},
	{VPK_ACTION_ACCEINFO,					"ACCE",		"tima -accinfo "},
	{VPK_ACTION_SNAPSHOT,					"SNAP",		"tima -snapshot "},
	{VPK_ACTION_MOVIEREC,					"VIDEO",	"tima -movierec "},
	{VPK_ACTION_MOVIELEN,					"VIDEO",	"tima -movielen "},
	{VPK_ACTION_RECSTATE,					"VIDEO",	"tima -recstate "},
	{VPK_ACTION_DEVINFO,					"DEVINFO",	"tima -getdevinfo "},
	{VPK_ACTION_VIDEOREC,					"VIDEO",	"tima -getvideo "},
	{VPK_ACTION_GETFILENAME,				"FILE",		"tima -getcurfile "},
	{VPK_ACTION_UNBINDEVICE,				"UNBIND",	"tima -removebind "},

	/* menu get */
	{VPK_ACTION_MENU_PICSIZEGET,			"MENUGET",	"tima -picsizeget "},
	{VPK_ACTION_MENU_RECSIZEGET,			"MENUGET",	"tima -recsizeget "},
	{VPK_ACTION_MENU_CYCRECGET,				"MENUGET",	"tima -cycrecget "},
	{VPK_ACTION_MENU_GSENSORGET,			"MENUGET",	"tima -gsensorget "},
	{VPK_ACTION_MENU_PARKGSENSORGET,		"MENUGET",	"tima -park_gsensorget "},
	{VPK_ACTION_MENU_PARKMONITORGET,		"MENUGET",	"tima -parkmonitorget "},
	{VPK_ACTION_MENU_POWEROFFVOLTGET,		"MENUGET",	"tima -poweroffvoltget "},
	{VPK_ACTION_MENU_AUDIOGET,				"MENUGET",	"tima -audioget "},
	{VPK_ACTION_MENU_HDRGET,				"MENUGET",	"tima -hdrget "},
	{VPK_ACTION_MENU_TIMESTAMPGET,			"MENUGET",	"tima -timestampget "},
	{VPK_ACTION_MENU_UPDATEGET,				"MENUGET",	"tima -updateget "},
	{VPK_ACTION_MENU_POWERSTATEGET,			"MENUGET",	"tima -powerstateget "},
	{VPK_ACTION_MENU_CRASHSTATGET,			"MENUGET",	"tima -crashstatget "},
	{VPK_ACTION_MENU_DRIVEBEHAVIORGET,		"MENUGET",	"tima -drivebehaviorget "},
	{VPK_ACTION_MENU_DEVSTATGET,			"MENUGET",	"tima -devstatget "},
	{VPK_ACTION_MENU_FORMATSD_NOP,			"MENUGET",	" "},	// be careful
	{VPK_ACTION_MENU_FACTORYSET_NOP,		"MENUGET",	" "},	// be careful
	{VPK_ACTION_MENU_VERSIONGET,			"MENUGET",	"tima -versionget "},
	{VPK_ACTION_MENU_WIFIPHRASEGET,			"MENUGET",	"tima -wifiphraseget "},

	/* menu set */
	{VPK_ACTION_MENU_PICSIZESET,			"MENUSET",  "tima -picsizeset "},
	{VPK_ACTION_MENU_RECSIZESET,			"MENUSET",  "tima -recsizeset "},
	{VPK_ACTION_MENU_CYCRECSET,				"MENUSET",  "tima -cycrecset "},
	{VPK_ACTION_MENU_GSENSORSET,			"MENUSET",  "tima -gsensorset "},
	{VPK_ACTION_MENU_PARKGSENSORSET,		"MENUGET",	"tima -park_gsensorset "},
	{VPK_ACTION_MENU_PARKMONITORSET,		"MENUSET",  "tima -parkmonitorset "},
	{VPK_ACTION_MENU_POWEROFFVOLTSET,		"MENUSET",  "tima -poweroffvoltset "},
	{VPK_ACTION_MENU_AUDIOSET,				"MENUSET",  "tima -audioset "},
	{VPK_ACTION_MENU_HDRSET,				"MENUSET",  "tima -hdrset "},
	{VPK_ACTION_MENU_TIMESTAMPSET,			"MENUSET",  "tima -timestampset "},
	{VPK_ACTION_MENU_UPDATESET,				"MENUSET",  "tima -updateset "},
	{VPK_ACTION_MENU_POWERSTATESET,			"MENUSET",  "tima -powerstateset "},
	{VPK_ACTION_MENU_CRASHSTATSET,			"MENUSET",  "tima -crashstatset "},
	{VPK_ACTION_MENU_DRIVEBEHAVIORSET,		"MENUSET",  "tima -drivebehaviorset "},
	{VPK_ACTION_MENU_DEVSTATSET,			"MENUSET",  "tima -devstatset "},
	{VPK_ACTION_MENU_FORMATSD,				"MENUSET",  "tima -formatsd "},
	{VPK_ACTION_MENU_FACTORYSET,			"MENUSET",  "tima -factoryset "},
	{VPK_ACTION_MENU_VERSIONGET_NOP,		"MENUSET",  " "},		// not use
	{VPK_ACTION_MENU_WIFIPHRASESET,			"MENUSET",	"tima -wifiphraseset "},

	{VPK_ACTION_QRCODE,						"QRCODE",	"tima -qrcodeshow "},
	//	{VPK_ACTION_UPDATE_FREQGET,			"UPDATE",	"tima -updateconditionget "},
	{VPK_ACTION_UPDATE_WHETHER_DOWNLOAD,	"UPDATE",	"tima -fwdownload "},
	{VPK_ACTION_UPDATE_WHETHER_UPGRADE,		"UPDATE",	"tima -fwupdate "},

	/* tencent iot */
	{VPK_ACTION_IOTPIDGET,					"QQIOT",	"tima -getiotpid "},
	{VPK_ACTION_IOTIDSET,					"QQIOT",	"tima -setiotid "},
	{VPK_ACTION_IOTLICENCESET,				"QQIOT",	"tima -setiotlicence "},

	{VPK_ACTION_POWER_OFF,					"POWEROFF",	"tima -poweroff "},
};
const int vpk_action_tables_size = _countof(vpk_action_tables);

#if 1
static int tima_rpc_exec(int fd, const void *buf, void *recvbuf, size_t recvlen)
{
	int rc = 0;

	if (!recvbuf) {
		//fprintf(stderr, "recv buf is null pointer\n");
		LOG_E("recv buf is null pointer");
		return -1;
	}

	rc = tima_rpc_send(fd, buf, strlen(buf), 0);
	if (rc < 0) {
		LOG_E("tima_rpc_send error");
		return -1;
	}
	//printf("=== send rc = %d\n", rc);

	//memset(recvbuf, 0, sizeof(recvbuf));
	rc = tima_rpc_recv(fd, recvbuf, recvlen, 0);
	if (rc < 0) {
		LOG_E("tima_rpc_recv error");
		return -1;
	}

	return rc;
}
#else
static int tima_rpc_exec(int fd, const void *buf, void *recvbuf, size_t recvlen)
{
	int rc = 0;
	char *msg;

	if (!recvbuf) {
		//fprintf(stderr, "recv buf is null pointer\n");
		LOG_E("recv buf is null pointer");
		return -1;
	}

	rc = tima_rpc_send(fd, buf, strlen(buf), 0);
	if (rc < 0) {
		LOG_E("tima_rpc_send error");
		return -1;
	}

	rc = tima_rpc_recv_alloc(fd, &msg, 0);
	if (rc < 0) {
		LOG_E("tima_rpc_recv_alloc error");
		return -1;
	} else {
		int len = rc > recvlen - 1 ? recvlen - 1 : rc;
		memcpy(recvbuf, msg, len);
		tima_rpc_freemsg(msg);
		if (rc != len) {
			LOG_W("warn: recvbuf is less than received msg\n");
		}
	}

	return rc;
}
#endif

static int hi35xx_actuator_destruct(void *session)
{
	vpk_actuator_t* thiz = (vpk_actuator_t*)session;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("actuator \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			tima_rpc_close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->s_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_session_t* hi35xx_actuator_load(void *ctx)
{
	vpk_actuator_t* thiz = NULL;
	ctx = ctx;

	thiz = (vpk_actuator_t*)TIMA_MALLOC(sizeof(vpk_actuator_t));
	if (thiz)
	{
		thiz->destruct   = (vpk_destruct_func)hi35xx_actuator_destruct;
		thiz->data_buff  = NULL;
		thiz->total_size = 0;
		thiz->fd		 = -1;
	}

	return (vpk_session_t*)thiz;
}

static int hi35xx_actuator_open(vpk_session_t *session, void *data)
{
	vpk_actuator_t* thiz = (vpk_actuator_t*)session;
	return_val_if_fail(thiz != NULL, -1);

	thiz->fd = tima_rpc_open();
	if (thiz->fd < 0)
	{
		LOG_E("actuator \'%s\' communication open failed!", thiz->name);
		return thiz->fd;
	}

	return thiz->fd;
}

static int hi35xx_actuator_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	return 0;
}

static int hi35xx_actuator_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int ret = -1;
	vpk_actuator_t* thiz = (vpk_actuator_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0 && thiz->data_buff != NULL, -1);
	timout = timout;
	
	if (buf && nbytes > 0 && thiz->total_size > 0)
	{
		ret = tima_rpc_exec(thiz->fd, buf, thiz->data_buff, thiz->total_size);
	}

	return ret;
}

static int hi35xx_actuator_close(vpk_session_t *session)
{
	vpk_actuator_t* thiz = (vpk_actuator_t*)session;
	if (thiz && thiz->fd >= 0)
	{
		LOG_I("actuator \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		tima_rpc_close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("actuator(fd) has been closed!");
	}

	return 0;
}

static int hi35xx_actuator_opt_set(vpk_session_t *session)
{
	return 0;
}

/* The actuator operations */
static const session_ops hi35xx_actuator_ops =
{
	hi35xx_actuator_load,
	hi35xx_actuator_open,
	hi35xx_actuator_read,
	hi35xx_actuator_write,
	hi35xx_actuator_close,
	hi35xx_actuator_opt_set,
};

const session_ops* get_hi35xx_actuator_ops(void)
{
	return (const session_ops*)&hi35xx_actuator_ops;
}
