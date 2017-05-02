/**
 * History:
 * ================================================================
 * 2015-05-01 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "vpk.h"
#include "curl/curl.h"
// #include "vpk_logging.h"
// #include "vpk_system.h"

extern void *vpk_test2(void* arg);
// extern void *vpk_test3(void* arg);

typedef struct _PrivInfo
{
	pthread_mutex_t sample_mutex;
	sem_t			sample_sem;
}PrivInfo;

PrivInfo info;

int http_download(const char* remote_path, const char* local_path, long timout);

int main(int argc, char *argv[])
{
	int ret = 0;
	void* thread_result;
	pthread_t pth_test3, pth_test2;

	vpk_system_init(argc, argv);
	vpk_logging_level_set("DEBUG");

	//const char* remoteurl = "http://test.7xl1at.com2.z0.glb.qiniucdn.com/1493564584?e=1493568191&token=jB7K75I0zFaneS5mBfra-2hsnHU4XyxQdBL5dRuR:aCjaueUS_XywueLhp9TR4MYGFjU=";
	const char* remoteurl = "http://test.7xl1at.com2.z0.glb.qiniucdn.com/1493564584";
	const char* localpath = "./resource/123.mp4";
	http_download(remoteurl, localpath, 20);

// 	ret = pthread_create(&pth_test3, NULL, vpk_test3, (void*)NULL);
// 	if (ret != 0)
// 		LOG_E("create thread \'vpk_test3\' failed");

// 	ret = pthread_create(&pth_test2, NULL, vpk_test2, (void*)NULL);
// 	if (ret != 0)
// 		LOG_E("create thread \'vpk_test2\' failed");

// 	pthread_join(pth_test3, &thread_result);
// 	pthread_join(pth_test2, &thread_result);

	return 0;
}

size_t tool_header_cb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	//LOG_D("str: %s\n", ptr);
	return 0;
}


/* parse headers for Content-Length */
size_t get_contentlength_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int r;
	long len = 0;

	/* _snscanf() is Win32 specific */
	//r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);
	r = sscanf(ptr, "Content-Length: %ld\n", &len);

    LOG_D("len: %ld\n", len);
	if(r) /* Microsoft: we don't read the specs */
		*((long *) stream) = len;

	return size * nmemb;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

int http_download(const char* remote_path, const char* local_path, long timout)
{
	int ret = 0;
	CURLcode res;
	FILE* fp = NULL;
	CURL* curl_handle = NULL;
	char* redirect_url = NULL;
	long response_code;
	long file_size;

	return_val_if_fail(remote_path && local_path, -1);

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl_handle = curl_easy_init();
	if (!curl_handle)
	{
		curl_global_cleanup();
		return -1;
	}

	if((fp = fopen(local_path, "a+")) == NULL)
	{
		LOG_E("fopen failed!");
		curl_easy_cleanup(curl_handle);
		curl_global_cleanup();
		return -2;
	}

	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, remote_path);

	/* tell libcurl to don't follow redirection */
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);

	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	/* Switch on full protocol/debug output while testing */
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
#if 0
	/* disable progress meter, set to 0L to enable and disable debug output */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    /* write the data to this file handle. CURLOPT_FILE is also known as CURLOPT_WRITEDATA*/
    curl_easy_setopt(curl_handle, CURLOPT_FILE, fp);


    /* get it! */
    res = curl_easy_perform(curl_handle);
	if (res == CURLE_OK)
	{
		res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
		if((res == CURLE_OK) && ((response_code / 100) != 3)) {
			/* a redirect implies a 3xx response code */
			LOG_I("Not a redirect.\n");
		}
		else {
			res = curl_easy_getinfo(curl_handle, CURLINFO_REDIRECT_URL, &redirect_url);

			if((res == CURLE_OK) && redirect_url) {
				/* This is the new absolute URL that you could redirect to, even if
				* the Location: response header may have been a relative URL. */
				LOG_I("Redirected to: %s\n", redirect_url);
				curl_easy_setopt(curl_handle, CURLOPT_URL, redirect_url);
			}
			else
				LOG_E("CURLINFO_REDIRECT_URL error : %s\n", curl_easy_strerror(res));
		}
	}
	else
	{
		LOG_E("curl_easy_perform error : %s\n",curl_easy_strerror(res));
	}
#endif	
	LOG_I("remote_path: %s", remote_path);
	LOG_I("redirect_url: %s\n", redirect_url);


	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, get_contentlength_func);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &file_size);
	//curl_easy_setopt(curl_handle, CURLOPT_RESUME_FROM_LARGE, use_resume ? have_size: 0);
 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 	curl_easy_setopt(curl_handle, CURLOPT_FILE, fp);

	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timout);

 	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
 	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
  /* get it! */
    res = curl_easy_perform(curl_handle);
	if (res == CURLE_OK)
	{
		res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
		if (res != CURLE_OK) {
			LOG_E("CURLINFO_RESPONSE_CODE error : %s\n", curl_easy_strerror(res));
		}

		if (response_code == 404) {
			LOG_E("http download file not exsit!!!\n");
		}

		if((res == CURLE_OK) /*&& ((response_code / 100) != 3)*/) {
			/* a redirect implies a 3xx response code */
			LOG_I("response_code: %ld, file_size: %ld", response_code, file_size);
		}
	}
	else
	{
		LOG_E("curl_easy_perform error : %s\n",curl_easy_strerror(res));
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	fclose(fp);

	curl_global_cleanup();

	return 0;
}

//int test_nvtuctrl(const char* name)
//{
//	int ret = -1;
//	char recv_buf[256] = {0};
//	char cmd[256] = {0};
//	char cmd_2[256] = {0};
//	vpk_nvtuctrl_t* nvtuctrl = NULL;
//	return_val_if_fail(name != NULL, -1);
//
//	nvtuctrl = vpk_nvtuctrl_create(name);
//	return_val_if_fail(nvtuctrl != NULL, -1);
//
//	ret = vpk_nvtuctrl_open(nvtuctrl);
//	if (ret < 0)
//	{
//		LOG_E("open nvtuctrl \'%s\' failed.", name);
//	}
//
//	snprintf(cmd, sizeof(cmd), "ucustom -gpsinfo");
//	snprintf(cmd_2, sizeof(cmd_2), "ucustom -snapshot 1");
//
//	while (1)
//	{
//		ret = vpk_nvtuctrl_write(nvtuctrl, cmd, sizeof(cmd), recv_buf, sizeof(recv_buf), 0);
//		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
//		sleep(1);
//
//		ret = vpk_nvtuctrl_write(nvtuctrl, cmd_2, sizeof(cmd_2), recv_buf, sizeof(recv_buf), 0);
//		LOG_I("[%s] ret = %d, recv_buf: %s, len: %d", name, ret, recv_buf, strlen(recv_buf));
//		sleep(1);
//	}
//
//	vpk_nvtuctrl_close(nvtuctrl);
//	vpk_nvtuctrl_destroy(nvtuctrl);
//
//	return ret;
//}
//
//void *vpk_test3(void* arg)
//{
//	int ret = 0, recv_cnt = 0;
//	PrivInfo* thiz = &info;
//	ret = pthread_mutex_init(&thiz->sample_mutex, NULL);
//	if (ret != 0) 
//	{
//		LOG_E("sample_mutex init error.");
//		return NULL;
//	}
//	ret = sem_init(&thiz->sample_sem, 0, 0);
//	if (ret != 0)
//	{
//		pthread_mutex_destroy(&thiz->sample_mutex);
//		LOG_E("sample_sem init error.");
//		return NULL;
//	}
//
//	LOG_D("start test3 thread!");
//	while(1)
//	{
//		LOG_D("test3 thread run.");
//
//		ret = sem_wait(&thiz->sample_sem);
//		if (ret == 0)
//		{
//			recv_cnt++;
//			LOG_I("receive a sample_sem.");
//			pthread_mutex_lock(&thiz->sample_mutex);
//			//test_nvtuctrl("GPS1");
//			LOG_D("recv_cnt = %d, sleep 5\n\n", recv_cnt);
//			sleep(5);
//			pthread_mutex_unlock(&thiz->sample_mutex);
//		}
//		else
//		{
//			LOG_E("snap_sem wait error!");
//			sleep(2);
//		}
//
//		//sleep(1);
//	}
//}

static INLINE int sample_sem_post(PrivInfo *thiz)
{
	int ret = -1;
	return_val_if_fail(thiz != NULL, -1);

	ret = sem_post(&thiz->sample_sem);
	if (ret != 0)
		LOG_E("sample_sem post failed!");

	return ret;
}

void *vpk_test2(void* arg)
{
	sleep(3);
	LOG_D("start test2 thread!");
	int send_cnt = 0;
	PrivInfo* thiz = &info;
	while(1)
	{
		LOG_D("test2 thread run.");
		//pthread_mutex_lock(&thiz->sample_mutex);
		send_cnt++;
		LOG_D("send_cnt = %d, send a semaphore!\n", send_cnt);
		sample_sem_post(thiz);
		if (send_cnt == 6)
			break;
		//pthread_mutex_unlock(&thiz->sample_mutex);
		sleep(1);
	}

	LOG_D("==== end test2 thread! ====\n\n");

	return NULL;
}
