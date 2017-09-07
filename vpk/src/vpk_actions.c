/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include "vpk_actions.h"

#include "vpk_action.h"
#include "vpk_logging.h"
#include "vpk_nvtuctrlc.h"


typedef struct _PrivInfo
{
	vpk_nvtuctrl_t* nvtuctrl;
	char			name[16];
	char			action[256];
}PrivInfo;

typedef struct _ActionInfo
{
	VpkNvtuType type;

	char* name;
	char* action;
}ActionInfo;

static const ActionInfo action_tables[] =
{
	/** it needs order by 'VpkNvtuType' **/
	{VPK_NVTU_START_TAG,				"UCUSTOM",	"	"},
	{VPK_NVTU_GPSINFO,					"GPS",		"ucustom -gpsinfo "},
	{VPK_NVTU_ACCEINFO,					"ACCE",		"ucustom -accinfo "},
	{VPK_NVTU_SNAPSHOT,					"SNAP",		"ucustom -snapshot "},
	{VPK_NVTU_MOVIEREC,					"VIDEO",	"ucustom -movierec "},
	{VPK_NVTU_MOVIELEN,					"VIDEO",	"ucustom -movielen "},
	{VPK_NVTU_RECSTATE,					"VIDEO",	"ucustom -recstate "},
	{VPK_NVTU_DEVINFO,					"DEVINFO",	"ucustom -getdevinfo "},
	{VPK_NVTU_VIDEOREC,					"VIDEO",	"ucustom -getvideo "},

	/* menu get */
	{VPK_NVTU_MENU_PICSIZEGET,			"MENUGET",	"ucustom -picsizeget "},
	{VPK_NVTU_MENU_RECSIZEGET,			"MENUGET",	"ucustom -recsizeget "},
	{VPK_NVTU_MENU_CYCRECGET,			"MENUGET",	"ucustom -cycrecget "},
	{VPK_NVTU_MENU_GSENSORGET,			"MENUGET",	"ucustom -gsensorget "},
	{VPK_NVTU_MENU_PARKGSENSORGET,		"MENUGET",	"ucustom -park_gsensorget "},
	{VPK_NVTU_MENU_PARKMONITORGET,		"MENUGET",	"ucustom -parkmonitorget "},
	{VPK_NVTU_MENU_POWEROFFVOLTGET,		"MENUGET",	"ucustom -poweroffvoltget "},
	{VPK_NVTU_MENU_AUDIOGET,			"MENUGET",	"ucustom -audioget "},
	{VPK_NVTU_MENU_HDRGET,				"MENUGET",	"ucustom -hdrget "},
	{VPK_NVTU_MENU_TIMESTAMPGET,		"MENUGET",	"ucustom -timestampget "},
	{VPK_NVTU_MENU_UPDATEGET,			"MENUGET",	"ucustom -updateget "},
	{VPK_NVTU_MENU_POWERSTATEGET,		"MENUGET",	"ucustom -powerstateget "},
	{VPK_NVTU_MENU_CRASHSTATGET,		"MENUGET",	"ucustom -crashstatget "},
	{VPK_NVTU_MENU_DRIVEBEHAVIORGET,	"MENUGET",	"ucustom -drivebehaviorget "},
	{VPK_NVTU_MENU_DEVSTATGET,			"MENUGET",	"ucustom -devstatget "},
	{VPK_NVTU_MENU_FORMATSD_NOP,		"MENUGET",	" "},	// be careful
	{VPK_NVTU_MENU_FACTORYSET_NOP,		"MENUGET",	" "},	// be careful
	{VPK_NVTU_MENU_VERSIONGET,			"MENUGET",	"ucustom -versionget "},
	{VPK_NVTU_MENU_WIFIPHRASEGET,		"MENUGET",	"ucustom -wifiphraseget "},

	/* menu set */
	{VPK_NVTU_MENU_PICSIZESET,			"MENUSET",  "ucustom -picsizeset "},
	{VPK_NVTU_MENU_RECSIZESET,			"MENUSET",  "ucustom -recsizeset "},
	{VPK_NVTU_MENU_CYCRECSET,			"MENUSET",  "ucustom -cycrecset "},
	{VPK_NVTU_MENU_GSENSORSET,			"MENUSET",  "ucustom -gsensorset "},
	{VPK_NVTU_MENU_PARKGSENSORSET,		"MENUGET",	"ucustom -park_gsensorset "},
	{VPK_NVTU_MENU_PARKMONITORSET,		"MENUSET",  "ucustom -parkmonitorset "},
	{VPK_NVTU_MENU_POWEROFFVOLTSET,		"MENUSET",  "ucustom -poweroffvoltset "},
	{VPK_NVTU_MENU_AUDIOSET,			"MENUSET",  "ucustom -audioset "},
	{VPK_NVTU_MENU_HDRSET,				"MENUSET",  "ucustom -hdrset "},
	{VPK_NVTU_MENU_TIMESTAMPSET,		"MENUSET",  "ucustom -timestampset "},
	{VPK_NVTU_MENU_UPDATESET,			"MENUSET",  "ucustom -updateset "},
	{VPK_NVTU_MENU_POWERSTATESET,		"MENUSET",  "ucustom -powerstateset "},
	{VPK_NVTU_MENU_CRASHSTATSET,		"MENUSET",  "ucustom -crashstatset "},
	{VPK_NVTU_MENU_DRIVEBEHAVIORSET,	"MENUSET",  "ucustom -drivebehaviorset "},
	{VPK_NVTU_MENU_DEVSTATSET,			"MENUSET",  "ucustom -devstatset "},
	{VPK_NVTU_MENU_FORMATSD,			"MENUSET",  "ucustom -formatsd "},
	{VPK_NVTU_MENU_FACTORYSET,			"MENUSET",  "ucustom -factoryset "},
	{VPK_NVTU_MENU_VERSIONGET_NOP,		"MENUSET",  " "},		// not use
	{VPK_NVTU_MENU_WIFIPHRASESET,		"MENUSET",	"ucustom -wifiphraseset "},

	{VPK_NVTU_QRCODE,					"QRCODE",	"ucustom -qrcodeshow "},
//	{VPK_NVTU_UPDATE_FREQGET,			"UPDATE",	"ucustom -updateconditionget "},
	{VPK_NVTU_UPDATE_WHETHER_DOWNLOAD,	"UPDATE",	"ucustom -fwdownload "},
	{VPK_NVTU_UPDATE_WHETHER_UPGRADE,	"UPDATE",	"ucustom -fwupdate "},

	/* tencent iot */
	{VPK_NVTU_IOTPIDGET,				"QQIOT",	"ucustom -getiotpid "},
	{VPK_NVTU_IOTIDSET,					"QQIOT",	"ucustom -setiotid "},
	{VPK_NVTU_IOTLICENCESET,			"QQIOT",	"ucustom -setiotlicence "},

	{VPK_NVTU_POWER_OFF,				"POWEROFF",	"ucustom -poweroff "},
};

VpkAction* vpk_action_create(VpkNvtuType type, void *param, void *recvbuf, uint32_t recvsize) 
{
	int i = 0, ret = -1;
	VpkAction* thiz = NULL;
	return_val_if_fail(recvbuf != NULL, NULL);

	thiz = (VpkAction*)TIMA_MALLOC(sizeof(VpkAction) + sizeof(PrivInfo));
	if (thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		memset(thiz, 0x00, sizeof(VpkAction) + sizeof(PrivInfo));

		if (type == action_tables[type-VPK_NVTU_START_TAG].type) 
		{
			thiz->type = type;
			strcpy(priv->name, action_tables[type-VPK_NVTU_START_TAG].name);
			strncpy(priv->action, action_tables[type-VPK_NVTU_START_TAG].action, strlen(action_tables[type-VPK_NVTU_START_TAG].action));
		}
		else
		{
			for (i = 0; i < _countof(action_tables); i++)
			{
				if (type == action_tables[i].type) {
					thiz->type = type;
					strcpy(priv->name, action_tables[i].name);
					strncpy(priv->action, action_tables[i].action, strlen(action_tables[i].action));
					break;
				}
			}
		}

		if (thiz->type == 0)
		{
			TIMA_FREE(thiz);
			LOG_W("The cmd or instructions[type:%d] unrecognized and return!!!", type);
			return NULL;
		}

		thiz->recvbuf	= recvbuf;
		thiz->recvsize	= recvsize;
		if (param != NULL)
		{
			/** "ucustom -snapshot 1" **/
			sprintf(priv->action, "%s %s", priv->action, (char*)param);
		}

		LOG_D("[%s]action or instructions created: \'%s\'", priv->name, priv->action);

		priv->nvtuctrl = vpk_nvtuctrl_create(priv->name);
		if (priv->nvtuctrl == NULL)
		{
			TIMA_FREE(thiz);
			LOG_E("nvtuctrl create failed!");
			return NULL;
		}

		ret = vpk_nvtuctrl_open(priv->nvtuctrl);
		if (ret < 0)
		{
			LOG_E("open nvtuctrl \'%s\' failed.", priv->name);
			vpk_nvtuctrl_destroy(priv->nvtuctrl);
			TIMA_FREE(thiz);
			return NULL;
		}
	}

	return thiz;
}

int vpk_action_param_set(VpkAction* thiz, VpkNvtuType type, void *param)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, -1);

// 	if (thiz->type != type)
// 	{
// 		for (int i = 0; i < _countof(action_tables); i++) 
// 		{
// 			if (type == action_tables[i].type) 
// 			{
// 				thiz->type = type;
// 				strcpy(priv->name, action_tables[i].name);
// 				strncpy(priv->action, action_tables[i].action, strlen(action_tables[i].action));
// 				break;
// 			}
// 		}
// 	}

	if (type == action_tables[type-VPK_NVTU_START_TAG].type) 
	{
		memset(priv->name, 0x00, sizeof(priv->name));
		memset(priv->action, 0x00, sizeof(priv->action));
		thiz->type = type;
		strcpy(priv->name, action_tables[type-VPK_NVTU_START_TAG].name);
		strncpy(priv->action, action_tables[type-VPK_NVTU_START_TAG].action, strlen(action_tables[type-VPK_NVTU_START_TAG].action));
	}
	else
	{
		for (i = 1; i < _countof(action_tables); i++) 
		{
			if (type == action_tables[i].type) 
			{
				memset(priv->name, 0x00, sizeof(priv->name));
				memset(priv->action, 0x00, sizeof(priv->action));
				thiz->type = type;
				strcpy(priv->name, action_tables[i].name);
				strncpy(priv->action, action_tables[i].action, strlen(action_tables[i].action));
				break;
			}
		}

		if (i == _countof(action_tables))
		{
			LOG_W("failed set the cmd or instructions[type:%d] unrecognized!!!", type);
			return -1;
		}
	}

	if (param != NULL) 
	{
		/** "ucustom -snapshot 1" **/
		sprintf(priv->action, "%s %s", priv->action, (char*)param);
	}

	LOG_D("set action[%s]: \'%s\'", priv->name, priv->action);

	return 0;
}

int vpk_action_exec(VpkAction* thiz, VpkActionCallback callback, void *ctx)
{
	int ret = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && thiz->recvbuf != NULL, -1);

	memset(thiz->recvbuf, 0x00, thiz->recvsize);
	ret = vpk_nvtuctrl_write(priv->nvtuctrl, priv->action, strlen(priv->action), thiz->recvbuf, thiz->recvsize, 0);
	if (!strstr(priv->action, "gpsinfo"))
	LOG_D("action[%s] ret = %d, recv_buf: %s, len: %d", priv->name, ret, thiz->recvbuf, strlen(thiz->recvbuf));
	if (callback == NULL)
	{
		LOG_E("callback is NULL!");
		return -1;
	}

	if (callback(ctx, (void*)thiz->recvbuf) < 0)
		return -1;
	//sleep(1);

	return ret;
}

int vpk_action_destroy(VpkAction* thiz) 
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, -1);

	if (priv->nvtuctrl)
	{
		vpk_nvtuctrl_close(priv->nvtuctrl);
		vpk_nvtuctrl_destroy(priv->nvtuctrl);
		priv->nvtuctrl = NULL;
	}

	thiz->param		= NULL;
	thiz->recvbuf	= NULL;
	TIMA_FREE(thiz);

	return 0;
}
