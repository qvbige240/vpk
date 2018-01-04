/*
 * History:
 * ================================================================
 * 2017-03-27 qing.zou created
 *
 */
#include <unistd.h>

#include "nvtuctrl.h"

#include "vpk_logging.h"
#include "nt966x_nvtuctrl.h"

const ActionInfo vpk_action_tables[] =
{
	/** it needs order by 'VpkActionType' **/
	{VPK_ACTION_START_TAG,				"UCUSTOM",	"	"},
	{VPK_ACTION_GPSINFO,					"GPS",		"ucustom -gpsinfo "},
	{VPK_ACTION_ACCEINFO,					"ACCE",		"ucustom -accinfo "},
	{VPK_ACTION_SNAPSHOT,					"SNAP",		"ucustom -snapshot "},
	{VPK_ACTION_MOVIEREC,					"VIDEO",	"ucustom -movierec "},
	{VPK_ACTION_MOVIELEN,					"VIDEO",	"ucustom -movielen "},
	{VPK_ACTION_RECSTATE,					"VIDEO",	"ucustom -recstate "},
	{VPK_ACTION_DEVINFO,					"DEVINFO",	"ucustom -getdevinfo "},
	{VPK_ACTION_VIDEOREC,					"VIDEO",	"ucustom -getvideo "},
	{VPK_ACTION_GETFILENAME,				"FILE",		"ucustom -getcurfile "},
	{VPK_ACTION_UNBINDEVICE,				"UNBIND",	"ucustom -removebind "},

	/* menu get */
	{VPK_ACTION_MENU_PICSIZEGET,			"MENUGET",	"ucustom -picsizeget "},
	{VPK_ACTION_MENU_RECSIZEGET,			"MENUGET",	"ucustom -recsizeget "},
	{VPK_ACTION_MENU_CYCRECGET,			"MENUGET",	"ucustom -cycrecget "},
	{VPK_ACTION_MENU_GSENSORGET,			"MENUGET",	"ucustom -gsensorget "},
	{VPK_ACTION_MENU_PARKGSENSORGET,		"MENUGET",	"ucustom -park_gsensorget "},
	{VPK_ACTION_MENU_PARKMONITORGET,		"MENUGET",	"ucustom -parkmonitorget "},
	{VPK_ACTION_MENU_POWEROFFVOLTGET,		"MENUGET",	"ucustom -poweroffvoltget "},
	{VPK_ACTION_MENU_AUDIOGET,			"MENUGET",	"ucustom -audioget "},
	{VPK_ACTION_MENU_HDRGET,				"MENUGET",	"ucustom -hdrget "},
	{VPK_ACTION_MENU_TIMESTAMPGET,		"MENUGET",	"ucustom -timestampget "},
	{VPK_ACTION_MENU_UPDATEGET,			"MENUGET",	"ucustom -updateget "},
	{VPK_ACTION_MENU_POWERSTATEGET,		"MENUGET",	"ucustom -powerstateget "},
	{VPK_ACTION_MENU_CRASHSTATGET,		"MENUGET",	"ucustom -crashstatget "},
	{VPK_ACTION_MENU_DRIVEBEHAVIORGET,	"MENUGET",	"ucustom -drivebehaviorget "},
	{VPK_ACTION_MENU_DEVSTATGET,			"MENUGET",	"ucustom -devstatget "},
	{VPK_ACTION_MENU_FORMATSD_NOP,		"MENUGET",	" "},	// be careful
	{VPK_ACTION_MENU_FACTORYSET_NOP,		"MENUGET",	" "},	// be careful
	{VPK_ACTION_MENU_VERSIONGET,			"MENUGET",	"ucustom -versionget "},
	{VPK_ACTION_MENU_WIFIPHRASEGET,		"MENUGET",	"ucustom -wifiphraseget "},

	/* menu set */
	{VPK_ACTION_MENU_PICSIZESET,			"MENUSET",  "ucustom -picsizeset "},
	{VPK_ACTION_MENU_RECSIZESET,			"MENUSET",  "ucustom -recsizeset "},
	{VPK_ACTION_MENU_CYCRECSET,			"MENUSET",  "ucustom -cycrecset "},
	{VPK_ACTION_MENU_GSENSORSET,			"MENUSET",  "ucustom -gsensorset "},
	{VPK_ACTION_MENU_PARKGSENSORSET,		"MENUGET",	"ucustom -park_gsensorset "},
	{VPK_ACTION_MENU_PARKMONITORSET,		"MENUSET",  "ucustom -parkmonitorset "},
	{VPK_ACTION_MENU_POWEROFFVOLTSET,		"MENUSET",  "ucustom -poweroffvoltset "},
	{VPK_ACTION_MENU_AUDIOSET,			"MENUSET",  "ucustom -audioset "},
	{VPK_ACTION_MENU_HDRSET,				"MENUSET",  "ucustom -hdrset "},
	{VPK_ACTION_MENU_TIMESTAMPSET,		"MENUSET",  "ucustom -timestampset "},
	{VPK_ACTION_MENU_UPDATESET,			"MENUSET",  "ucustom -updateset "},
	{VPK_ACTION_MENU_POWERSTATESET,		"MENUSET",  "ucustom -powerstateset "},
	{VPK_ACTION_MENU_CRASHSTATSET,		"MENUSET",  "ucustom -crashstatset "},
	{VPK_ACTION_MENU_DRIVEBEHAVIORSET,	"MENUSET",  "ucustom -drivebehaviorset "},
	{VPK_ACTION_MENU_DEVSTATSET,			"MENUSET",  "ucustom -devstatset "},
	{VPK_ACTION_MENU_FORMATSD,			"MENUSET",  "ucustom -formatsd "},
	{VPK_ACTION_MENU_FACTORYSET,			"MENUSET",  "ucustom -factoryset "},
	{VPK_ACTION_MENU_VERSIONGET_NOP,		"MENUSET",  " "},		// not use
	{VPK_ACTION_MENU_WIFIPHRASESET,		"MENUSET",	"ucustom -wifiphraseset "},

	{VPK_ACTION_QRCODE,					"QRCODE",	"ucustom -qrcodeshow "},
	//	{VPK_ACTION_UPDATE_FREQGET,			"UPDATE",	"ucustom -updateconditionget "},
	{VPK_ACTION_UPDATE_WHETHER_DOWNLOAD,	"UPDATE",	"ucustom -fwdownload "},
	{VPK_ACTION_UPDATE_WHETHER_UPGRADE,	"UPDATE",	"ucustom -fwupdate "},

	/* tencent iot */
	{VPK_ACTION_IOTPIDGET,				"QQIOT",	"ucustom -getiotpid "},
	{VPK_ACTION_IOTIDSET,					"QQIOT",	"ucustom -setiotid "},
	{VPK_ACTION_IOTLICENCESET,			"QQIOT",	"ucustom -setiotlicence "},

	{VPK_ACTION_POWER_OFF,				"POWEROFF",	"ucustom -poweroff "},
};
const int vpk_action_tables_size = _countof(vpk_action_tables);

static int nt966x_nvtuctrl_destruct(void *session)
{
	vpk_actuator_t* thiz = (vpk_actuator_t*)session;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			NvtUctrl_Close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->s_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_session_t* nt966x_nvtuctrl_load(void *ctx)
{
	vpk_actuator_t* thiz = NULL;
	ctx = ctx;

	thiz = (vpk_actuator_t*)TIMA_MALLOC(sizeof(vpk_actuator_t));
	if (thiz)
	{
		thiz->destruct   = (vpk_destruct_func)nt966x_nvtuctrl_destruct;
		thiz->data_buff  = NULL;
		thiz->total_size = 0;
		thiz->fd		 = -1;
	}

	return (vpk_session_t*)thiz;
}

static int nt966x_nvtuctrl_open(vpk_session_t *session, void *data)
{
	vpk_actuator_t* thiz = (vpk_actuator_t*)session;
	return_val_if_fail(thiz != NULL, -1);

	thiz->fd = NvtUctrl_Open();
	if (thiz->fd < 0)
	{
		LOG_E("nvtuctrl \'%s\' communication open failed!", thiz->name);
		return thiz->fd;
	}

	return thiz->fd;
}


static int nt966x_nvtuctrl_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	return 0;
}

static int nt966x_nvtuctrl_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int ret = -1;
	vpk_actuator_t* thiz = (vpk_actuator_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0 && thiz->data_buff != NULL, -1);
	timout = timout;
	
	if (buf && nbytes > 0 && thiz->total_size > 0)
	{
		ret = NvtUctrl_CmdSnd(thiz->fd, buf, thiz->data_buff, thiz->total_size);
	}

	return ret;
}

static int nt966x_nvtuctrl_close(vpk_session_t *session)
{
	vpk_actuator_t* thiz = (vpk_actuator_t*)session;
	if (thiz && thiz->fd >= 0)
	{
		LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		NvtUctrl_Close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("nvtuctrl(fd) has been closed!");
	}

	return 0;
}

static int nt966x_nvtuctrl_opt_set(vpk_session_t *session)
{
	return 0;
}

/* The nvtuctrl operations */
static const session_ops nt966x_nvtuctrl_ops =
{
	nt966x_nvtuctrl_load,
	nt966x_nvtuctrl_open,
	nt966x_nvtuctrl_read,
	nt966x_nvtuctrl_write,
	nt966x_nvtuctrl_close,
	nt966x_nvtuctrl_opt_set,
};

const session_ops* get_nt966x_nvtuctrl_ops(void)
{
	return (const session_ops*)&nt966x_nvtuctrl_ops;
}

