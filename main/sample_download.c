/**
 * History:
 * ================================================================
 * 2017-05-01 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
// #include <pthread.h>
// #include <semaphore.h>

#include "vpk.h"
#include "curl/curl.h"

int cnt_w = 0;
int total = 0;

int http_download(const char* remote_path, const char* local_path, long timout);

int download_main(int argc, char *argv[])
{

	char* remoteurl = "http://test.7xl1at.com2.z0.glb.qiniucdn.com/1493564584";
	//char* localpath = "./resource/123.mp4";
	char* localpath = "./FW966X.crdownload.mp4";

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);

		switch(o) {
			case 'f':
				localpath = optarg;
				break;
			case 'u':
				remoteurl = optarg;
				//LOG_D("keycode: %x", keycode);
				break;
			default:
				break;
		}
	}

	//if (argc > 2)
	//{
	//	localpath = argv[1];
	//	remoteurl = argv[2];
	//}

	LOG_D("localpath = %s, remoteurl = %s", localpath, remoteurl);
	http_download(remoteurl, localpath, 60);

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

	if(r) /* Microsoft: we don't read the specs */
	{
		*((long *) stream) = len;
		LOG_D("len: %ld, size: %ld, nmemb: %ld\n", len, size, nmemb);
	}

	return size * nmemb;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	cnt_w++;
	total += written;
	//LOG_D(" written: %d", written);
	return written;
}

int my_progress_func(void *bar,
					 double t, /* dltotal */
					 double d, /* dlnow */
					 double ultotal,
					 double ulnow)
{
	//printf("%f / %f (%g %%)\n", d, t, d*100.0/t);

	return 0;
}

int http_download(const char* remote_path, const char* local_path, long timout)
{
	//int ret = 0;
	CURLcode res;
	FILE* fp = NULL;
	CURL* curl_handle = NULL;
	//char* redirect_url = NULL;
	long response_code;
	long file_size = 0;
	long have_size = 0;
	int use_resume = 0;

	return_val_if_fail(remote_path && local_path, -1);

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl_handle = curl_easy_init();
	if (!curl_handle)
	{
		curl_global_cleanup();
		return -1;
	}

	if (vpk_exists(local_path))
	{
		have_size = vpk_file_length(local_path);
		use_resume = 1;
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
	//curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	/* Switch on full protocol/debug output while testing */
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
#if 0
	/* disable progress meter, set to 0L to enable and disable debug output */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	/* send all data to this function  */
	//curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    /* write the data to this file handle. CURLOPT_FILE is also known as CURLOPT_WRITEDATA*/
    //curl_easy_setopt(curl_handle, CURLOPT_FILE, fp);


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
	LOG_I("redirect_url: %s\n", redirect_url);
#endif	
	LOG_I("remote_path: %s", remote_path);

	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, get_contentlength_func);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &file_size);
	//curl_easy_setopt(curl_handle, CURLOPT_RESUME_FROM_LARGE, use_resume ? have_size: 0);
	if (use_resume)
		curl_easy_setopt(curl_handle, CURLOPT_RESUME_FROM, use_resume ? have_size: 0);
 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 	curl_easy_setopt(curl_handle, CURLOPT_FILE, fp);

	/* get progress bar */
	//curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, my_progress_func);
	/* set bar ctx */
	//curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, Bar);

	/* include the all time */
	//curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timout);

 	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
 	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
  
	/* get it! */
	res = curl_easy_perform(curl_handle);
	if (res == CURLE_OK)
	{
		double val;

		/* check for bytes downloaded */
		res = curl_easy_getinfo(curl_handle, CURLINFO_SIZE_DOWNLOAD, &val);
		if((CURLE_OK == res) && (val>0))
			printf("Data downloaded: %0.0f bytes.\n", val);

		/* check for total download time */
		res = curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME, &val);
		if((CURLE_OK == res) && (val>0))
			printf("Total download time: %0.3f sec.\n", val);

		/* check for average download speed */
		res = curl_easy_getinfo(curl_handle, CURLINFO_SPEED_DOWNLOAD, &val);
		if((CURLE_OK == res) && (val>0))
			printf("Average download speed: %0.3f kbyte/sec.\n", val / 1024);

		if(1) {
			/* check for name resolution time */
			res = curl_easy_getinfo(curl_handle, CURLINFO_NAMELOOKUP_TIME, &val);
			if((CURLE_OK == res) && (val>0))
				printf("Name lookup time: %0.3f sec.\n", val);

			/* check for connect time */
			res = curl_easy_getinfo(curl_handle, CURLINFO_CONNECT_TIME, &val);
			if((CURLE_OK == res) && (val>0))
				printf("Connect time: %0.3f sec.\n", val);
		}


		res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
		if (res != CURLE_OK) {
			LOG_E("CURLINFO_RESPONSE_CODE error : %s\n", curl_easy_strerror(res));
			goto error_end;
		}

		if (response_code == 404) {
			LOG_E("http download file not exsit!!!\n");
			goto error_end;
		}

		if((res == CURLE_OK) /*&& ((response_code / 100) != 3)*/) {
			/* a redirect implies a 3xx response code */
			LOG_I("response_code: %ld, file_size: %ld", response_code, file_size);
		}
	}
	else
	{
		LOG_E("curl_easy_perform error: network some error! (%s)\n", curl_easy_strerror(res));
		goto error_end;
	}

	LOG_D("write times(%d) count = %d", cnt_w, total);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	fclose(fp);

	curl_global_cleanup();

	return 0;

error_end:
	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	fclose(fp);

	curl_global_cleanup();
	return -1;
}
