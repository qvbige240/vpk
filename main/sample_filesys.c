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
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "vpk.h"
/*
static int file_length(const char* path)
{
#ifdef WIN32
	HANDLE hFile;
	DWORD dwFileSize = 0;

	hFile = CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		dwFileSize = GetFileSize(hFile,NULL);
		CloseHandle(hFile);
		return (int)dwFileSize;
	}
#else
	struct stat st;
	int ret = lstat(path, &st);
	if(ret < 0)
		return ret;

	if(S_ISREG(st.st_mode))
		return st.st_size;
#endif
	return -1;
}

static int file_exist(const char* path)
{
#ifdef WIN32
	DWORD dw;
	dw = GetFileAttributesA(path);
	if(dw != (DWORD)-1 && dw != FILE_ATTRIBUTE_DIRECTORY)
		return 1;
	return 0;
#else
	struct stat st;
	int ret = lstat(path, &st);
	if(ret < 0)
		return 0;

	if(S_ISREG(st.st_mode))
		return 1;

	return 0;
#endif
}

static int dir_exist(const char* path)
{
#ifdef WIN32
	DWORD dw;
	dw = GetFileAttributesA(path);
	if(dw != (DWORD)-1 && dw == FILE_ATTRIBUTE_DIRECTORY)
		return 1;
	return 0;
#else
	struct stat st;
	int ret = lstat(path, &st);
	if(ret < 0)
		return 0;

	if(S_ISDIR(st.st_mode))
		return 1;
	return 0;
#endif
}
*/
static void dir_print(void *ctx, const char *predir, const char *fname)
{
	char path[256] = {0};
	struct stat st;
	snprintf(path, sizeof(path), "%s/%s", predir, fname);
	int ret = lstat(path, &st);
	if (ret < 0) {
		LOG_E("lstat error ret = %d", ret);
		return;
	}

	printf("%s%07o  %9ld %ld %s\n", 
		S_ISDIR(st.st_mode) ? "d" : 
		S_ISREG(st.st_mode) ? "f" : 
		S_ISLNK(st.st_mode) ? "l" : "-", 
		st.st_mode, st.st_size, st.st_mtime, fname);
}

static int file_dir_test(const char *prefix) 
{
	int ret = -1;
	char tmpaa[256] = {0};
	char tmpbb[256] = {0};
	char tmpcc[256] = {0};
	char cmd[256] = {0};

	const char* path = prefix;
	snprintf(tmpaa, sizeof(tmpaa), "%s/aaaa/", path);
	snprintf(tmpbb, sizeof(tmpbb), "%s/bbbb", path);
	snprintf(tmpcc, sizeof(tmpcc), "%s/ctest", path);
	snprintf(cmd, sizeof(cmd), "touch %s", tmpcc);
	LOG_I("prefix path \'%s\'\n", path);

	ret = vpk_isdir(tmpaa);
	LOG_I("vpk_isdir \'%s\' ret = %d", tmpaa, ret);
	ret = vpk_isdir(path);
	LOG_I("vpk_isdir \'%s\' ret = %d\n", path, ret);

	ret = vpk_exists(path);
	LOG_I("vpk_exists \'%s\' ret = %d", path, ret);
	if (!ret) {
		ret = vpk_mkdir(path);
		LOG_I("path: \'%s\' not exist and create, ret = %d", path, ret);
	}
	ret = vpk_exists((const char*)tmpaa);
	LOG_I("vpk_exists \'%s\' ret = %d\n", tmpaa, ret);

	ret = vpk_isdir(path);
	LOG_I("vpk_isdir \'%s\' ret = %d", path, ret);
	//ret = vpk_isdir("/home/zouqing/test/c/aaaa");
	//LOG_I("vpk_isdir \'/home/zouqing/test/c/aaaa\' ret = %d\n", ret);

	ret = vpk_mkdir((const char*)tmpaa);
	LOG_I("vpk_mkdir \'%s\' ret = %d", tmpaa, ret);

	ret = vpk_remove((const char*)tmpaa);
	LOG_I("vpk_remove \'%s\' ret = %d", tmpaa, ret);
	ret = vpk_remove((const char*)tmpbb);
	LOG_I("vpk_remove \'%s\' ret = %d\n", tmpbb, ret);

	vpk_enumerate(path, dir_print, NULL);

	vpk_system_ex(cmd, 3);

	ret = vpk_rename(tmpcc, tmpbb);
	LOG_I("vpk_rename \'%s\' to \'%s\' ret = %d\n", tmpcc, tmpbb, ret);

	printf("\r\n");
	vpk_enumerate(path, dir_print, NULL);

	if (vpk_exists(tmpcc)) {
		ret = vpk_remove(tmpcc);
		LOG_I("vpk_remove \'%s\' ret = %d\n", tmpcc, ret);
	}

	return 0;
}

int filesys_main(int argc, char *argv[])
{
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");
	char* localpath = "./tmp";
	//if (argc > 1)
	//{
	//	localpath = argv[1];
	//}

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
			default:
				break;
		}
	}

	LOG_D("file = %s", localpath);

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	file_dir_test((const char*)localpath);

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);
	//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
