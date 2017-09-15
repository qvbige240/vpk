/*
 * History:
 * ================================================================
 * 2017-03-27 qing.zou created
 *
 */
#include <unistd.h>

//#include "nvtuctrl.h"

#include "vpk_mmap.h"
#include "vpk_logging.h"
#include "x86_nvtuctrl.h"

#include "jansson.h"

static int x86_nvtuctrl_destruct(void *session)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			//NvtUctrl_Close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->s_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_session_t* x86_nvtuctrl_load(void *ctx)
{
	vpk_nvtuctrl_t* thiz = NULL;
	ctx = ctx;

	thiz = (vpk_nvtuctrl_t*)TIMA_MALLOC(sizeof(vpk_nvtuctrl_t));
	if (thiz)
	{
		thiz->destruct   = (vpk_destruct_func)x86_nvtuctrl_destruct;
		thiz->data_buff  = NULL;
		thiz->total_size = 0;
		thiz->fd		 = -1;
	}

	return (vpk_session_t*)thiz;
}

static int x86_nvtuctrl_open(vpk_session_t *session, void *data)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	return_val_if_fail(thiz != NULL, -1);

	//thiz->fd = NvtUctrl_Open();
	thiz->fd = 1;
	if (thiz->fd < 0)
	{
		LOG_E("nvtuctrl \'%s\' communication open failed!", thiz->name);
		return thiz->fd;
	}

	return thiz->fd;
}


static int x86_nvtuctrl_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	return 0;
}

static int setting_item_get(const char* file, char* key, int* value)
{
	int ret = 0;
	json_t* json_root = NULL;
	if (vpk_mmap_exist(file)) {
		json_root = json_load_file(file, 0, NULL);
	}
	else {
		LOG_W("file not exist, and new json_root");
		json_root = json_object();
	}

	if (json_root)
	{
		json_t* jobject = json_object_get(json_root, key);
		if (!jobject)
		{
			LOG_W("jobject not exist, and return default 0.");
			*value = 0;
			ret = -1;
		}
		else
		{
			*value = json_integer_value(jobject);
		}

		json_decref(json_root);
	}

	return ret;
}

static int setting_item_write(const char* file, char* key, int value)
{
	json_t* json_root = NULL;
	if (vpk_mmap_exist(file)) {
		json_root = json_load_file(file, 0, NULL);
	}
	else {
		LOG_W("file not exist, and new json_root");
		json_root = json_object();
	}

	if (json_root)
	{
		json_t* jobject = json_object_get(json_root, key);
		if (!jobject)
		{
			LOG_W("jobject not exist, and new json object.");
			json_object_set(json_root, key, json_integer(value));
			json_dump_file(json_root, file, 0);						/* to file */

			char* update_dump = json_dumps(json_root, 0);			/* to buffer */
			LOG_D("update_dump setting:\n%s\n", update_dump);
			free(update_dump);
		}
		else
		{
			int curr_val = json_integer_value(jobject);
			if (curr_val != value)
			{
				json_object_set(json_root, key, json_integer(value));
				json_dump_file(json_root, file, 0);						/* to file */

				char* update_dump = json_dumps(json_root, 0);			/* to buffer */
				LOG_D("update_dump setting:\n%s\n", update_dump);
				free(update_dump);
			}
			else
			{
				LOG_D("no value change at \'%s\'", key);
			}
		}

		json_decref(json_root);
	}

	return 0;
}

static char gps_str[256] = {0};
static int x86_nvtuctrl_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int ret = -1;
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0 && thiz->data_buff != NULL, -1);
	timout = timout;
	
	// fill the receive buffer with test data
	if (buf && nbytes > 0 && thiz->total_size > 0)
	{
		//ret = NvtUctrl_CmdSnd(thiz->fd, buf, thiz->data_buff, thiz->total_size);
		char *p = NULL;
		//GPS
		p = strstr(buf, "-gpsinfo");
		if (p != NULL) 
		{
			//char *str = "{\"Latitude\":30.657300,\"Longitude\":104.065855,\"Speed\":0.300003,\"Angle\":0.400004}";
			//char *str = "{\"Latitude\":0.100001,\"Longitude\":0.200002,\"Speed\":0.300003,\"Angle\":0.400004}";
			//memcpy(thiz->data_buff, str, strlen(str));

			#define DEVICE_GPS_INFO_FILE	"dev_gps_pc.json"
			if (!strlen(gps_str)) {
				ret = vpk_mmap_exist(DEVICE_GPS_INFO_FILE);
				VpkMmap* mmap = vpk_mmap_create(DEVICE_GPS_INFO_FILE, 0, -1);
				char* str = vpk_mmap_data(mmap);
				strncpy(gps_str, str, sizeof(gps_str));
				LOG_D("read GPS json: %s", gps_str);
				vpk_mmap_destroy(mmap);
			}
			memcpy(thiz->data_buff, gps_str, strlen(gps_str));

			return 0;
		}
		// SNAP
		p = strstr(buf, "-snapshot");
		if (p != NULL)
		{
			char *str = "resources/test_2017_0409_171946_RE.JPG";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		// VIDEO
		p = strstr(buf, "-movierec");
		if (p != NULL)
		{
			char *str = "resources/test_2017.mp4";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-movielen");
		if (p != NULL)
		{
			char *str = "movie len:8288";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-getdevinfo");
		if (p != NULL)
		{
			#define DEVICE_RELEASE_INFO_FILE	"dev_info_pc.json"
			ret = vpk_mmap_exist(DEVICE_RELEASE_INFO_FILE);
			VpkMmap* mmap = vpk_mmap_create(DEVICE_RELEASE_INFO_FILE, 0, -1);
			char* str = vpk_mmap_data(mmap);
			memcpy(thiz->data_buff, str, strlen(str));
			vpk_mmap_destroy(mmap);
			return 0;
		}
		p = strstr(buf, "-qrcodeshow");
		if (p != NULL)
		{
			return 0;
		}
		p = strstr(buf, "-setiotid");
		if (p != NULL)
		{
			return 0;
		}
		p = strstr(buf, "-setiotlicence");
		if (p != NULL)
		{
			return 0;
		}
		p = strstr(buf, "-updateconditionget");
		if (p != NULL)
		{
			char *str = "1";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-fwdownload");
		if (p != NULL)
		{
			//char *str = "1";
			//memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-fwupdate");
		if (p != NULL)
		{
			return 0;
		}
		p = strstr(buf, "-poweroff");
		if (p != NULL)
		{
			return 0;
		}
		p = strstr(buf, "-recstate");
		if (p != NULL)
		{
			char *str = "0";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-getvideo");
		if (p != NULL)
		{
			char *str = "1";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-versionget");
		if (p != NULL)
		{
			char *str = "K40.20170808.01";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-getcurfile");
		if (p != NULL)
		{
			char *str = "2017_a.TS, 2017_b.TS";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-removebind");
		if (p != NULL)
		{
			char *str = "0";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "-wifiphraseget");
		if (p != NULL)
		{
			char *str = "DECAI_,%s";
			memcpy(thiz->data_buff, str, strlen(str));
			return 0;
		}
		p = strstr(buf, "MENUMOCK");		/* menus setting mock */
		if (p != NULL)
		{
			#define DEVICE_MENUS_INFO_FILE	"menu_setting_pc.json"
			char* key = NULL;
			int value = 0;
			char tmp[256] = {0};
			int r = sscanf(buf, "ucustom -%s MENUMOCK\n", tmp);
			if (r) {
				p = strstr(tmp, "set");
				if (p)
				{
					int pos = p - tmp;
					tmp[pos] = '\0';
					key = tmp;
					p = strstr(buf, "MENUMOCK");
					if (p)
					{
						p = p + strlen("MENUMOCK");
						value = atoi(p);
						LOG_D("set key = %s, value(atoi) = %d\n", key, value);
						setting_item_write(DEVICE_MENUS_INFO_FILE, key, value);
						sprintf(thiz->data_buff, "%d", value);
						//memcpy(thiz->data_buff, &value, sizeof(int));
					}
				}
				p = strstr(tmp, "get");
				if (p)
				{
					int pos = p - tmp;
					tmp[pos] = '\0';
					key = tmp;
					setting_item_get(DEVICE_MENUS_INFO_FILE, key, &value);
					LOG_D("get key = %s, value = %d\n", key, value);
					sprintf(thiz->data_buff, "%d", value);
					//memcpy(thiz->data_buff, &value, sizeof(int));
				}
				else
				{
					char *str = "0";
					memcpy(thiz->data_buff, str, strlen(str));
				}
			}				
			else {
				char *str = "0";
				memcpy(thiz->data_buff, str, strlen(str));
			}


			return 0;
		}
		else
		{
			char *str = "unkonwn command!!!";
			memcpy(thiz->data_buff, str, strlen(str));
		}
		
	}

	return ret;
}

static int x86_nvtuctrl_close(vpk_session_t *session)
{
	vpk_nvtuctrl_t* thiz = (vpk_nvtuctrl_t*)session;
	if (thiz && thiz->fd >= 0)
	{
		LOG_I("nvtuctrl \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		//NvtUctrl_Close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("nvtuctrl(fd) has been closed!");
	}

	return 0;
}

static int x86_nvtuctrl_opt_set(vpk_session_t *session)
{
	return 0;
}

/* The nvtuctrl operations */
static const session_ops x86_nvtuctrl_ops =
{
	x86_nvtuctrl_load,
	x86_nvtuctrl_open,
	x86_nvtuctrl_read,
	x86_nvtuctrl_write,
	x86_nvtuctrl_close,
	x86_nvtuctrl_opt_set,
};

const session_ops* get_x86_nvtuctrl_ops(void)
{
	return (const session_ops*)&x86_nvtuctrl_ops;
}

