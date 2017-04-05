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
	char			action[128];
}PrivInfo;

typedef struct _ActionInfo
{
	VpkNvtuType type;

	char name[16];
	char action[32];
}ActionInfo;

static ActionInfo action_tables[] =
{
	{VPK_NVTU_GPSINFO,	"GPS",	"ucustom -gpsinfo "},
	{VPK_NVTU_SNAPSHOT,	"SNAP",	"ucustom -snapshot "},

	//	{VPK_NVTU_ACCINFO, "ACC", "ucustom -gpsinfo",},		//
	//{VPK_NVTU_MAX,	"SNAP",	"ucustom -snapshot ",	NULL},
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

		for (i = 0; i < _countof(action_tables); i++)
		{
			if (type == action_tables[i].type) {
				thiz->type = type;
				strcpy(priv->name, action_tables[i].name);
				strncpy(priv->action, action_tables[i].action, strlen(action_tables[i].action));
				break;
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
			sprintf(priv->action, " %s", param);

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

	for (i = 0; i < _countof(action_tables); i++) 
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

	if (param != NULL) 
	{
		sprintf(priv->action, " %s", param);
	}

	LOG_D("action[%s]: \'%s\' set successful!", priv->name, priv->action);

	return 0;
}

int vpk_action_exec(VpkAction* thiz, VpkActionCallback callback, void *ctx)
{
	int ret = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && thiz->recvbuf != NULL, -1);

	memset(thiz->recvbuf, 0x00, thiz->recvsize);
	ret = vpk_nvtuctrl_write(priv->nvtuctrl, priv->action, strlen(priv->action), thiz->recvbuf, thiz->recvsize, 0);
	LOG_D("[%s] ret = %d, recv_buf: %s, len: %d", priv->name, ret, thiz->recvbuf, strlen(thiz->recvbuf));
	if (callback == NULL)
	{
		LOG_E("callback is NULL!");
		return -1;
	}

	callback(ctx, (void*)thiz->recvbuf);
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
