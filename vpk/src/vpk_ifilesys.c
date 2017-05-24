/*
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "vpk_filesys.h"
#include "vpk_logging.h"
#include "vpk_ifilesys.h"

/*
static vpk_filesys_t* filesys = NULL;

int vpk_filesys_init(void)
{
	const filesys_ops* lops = NULL;

	vpk_system_t* vpk_sys = vpk_system();
	return_val_if_fail(vpk_sys != NULL, -1);

	fops = vpk_sys->sys_kset->get_filesys_ops();
	return_val_if_fail(fops != NULL, -1);

	filesys = fops->init();
	return_val_if_fail(filesys != NULL, -1);
	filesys->fops = fops;

	return 0;
}

vpk_filesys_t* vpk_filesys(void)
{
	return filesys;
}
*/

VPKAPI int vpk_exists(const char* path)
{
	return access(path, 0) == 0;
}

VPKAPI long vpk_file_length(const char* path)
{
	struct stat st;
	memset(&st, 0x00, sizeof(struct stat));
	if (lstat(path, &st) < 0) {
		printf("file \'%s\': not exists or other.\n", path);
		return -1;
	}

	if(S_ISREG(st.st_mode))
		return st.st_size;

	return 0;
}

VPKAPI int vpk_isdir(const char* path)
{
	struct stat st;
	memset(&st, 0x00, sizeof(struct stat));
	if (lstat(path, &st) < 0) {
		printf("lstat error, directory \'%s\': not exists or other.\n", path);
		return 0;
	}
	//printf("%07o ", st.st_mode);
	return S_ISDIR(st.st_mode);
}

VPKAPI int vpk_mkdir(const char* path)
{
	struct stat st;

	if (stat(path, &st) != 0)
	{
		if (mkdir(path, 0777) != 0)
			return -1;
	}
	else if (!S_ISDIR(st.st_mode))
	{
		printf("mkdir: cannot create directory \'%s\': File exists\n", path);
		return -1;
	}

	return 0;
}

int vpk_filename_get(const char *full, char *name)
{
	const char *pos = strrchr(full, '/');
	if (pos == NULL)
		strcpy(name, full);
	else
		strcpy(name, pos+1);

	return 0;
}

VPKAPI int vpk_remove(const char* path)
{
	if (!vpk_exists(path)) {
		printf("%s file or directory already not exists: %s\n", __FUNCTION__, path);
		return -1;
	}
	return remove(path);
}

VPKAPI void vpk_enumerate(const char* folder, vpk_enumerate_callback callback, void* ctx)
{
	DIR* dir;
	char buf[FILENAME_MAX];
	struct dirent* dirent;

	dir = opendir(folder);
	if (!dir)
	{
		printf("%s open dir failed, dir: %s\n", __FUNCTION__, folder);
		//fprintf(stderr, "%s open dir failed, dir: %s\n", __FUNCTION__, folder);
		//fflush(stderr);
		return;
	}

	while (NULL != (dirent = readdir(dir)))
	{
		if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
			continue;

		snprintf(buf, sizeof(buf), "%s/%s", folder, dirent->d_name);
		if (!vpk_isdir(buf))
			callback(ctx, folder, dirent->d_name);
	}

	if (dir)
		closedir(dir);
}

/*
void dir_print(void *ctx, const char *predir, const char *fname)
{
	printf("predir: %s, file name: %s\n", predir, fname);
}
int main() 
{
	int ret = -1;
	
	ret = vpk_isdir("/home/zouqing/test/c/");
	printf("vpk_isdir \'/home/zouqing/test/c/\' ret = %d\n", ret);
	ret = vpk_isdir("/home/zouqing/test/c/aaaa");
	printf("vpk_isdir \'/home/zouqing/test/c/aaaa\' ret = %d\n", ret);

	ret = vpk_exists("/home/zouqing/test/c/");
	printf("vpk_exists \'/home/zouqing/test/c/\' ret = %d\n", ret);
	ret = vpk_exists("/home/zouqing/test/c/aaaa");
	printf("vpk_exists \'/home/zouqing/test/c/aaaa\' ret = %d\n", ret);

	ret = vpk_isdir("/home/zouqing/test/c/");
	printf("vpk_isdir \'/home/zouqing/test/c/\' ret = %d\n", ret);
	//ret = vpk_isdir("/home/zouqing/test/c/aaaa");
	//printf("vpk_isdir \'/home/zouqing/test/c/aaaa\' ret = %d\n", ret);

	ret = vpk_mkdir("/home/zouqing/test/c/aaaa");
	printf("vpk_mkdir \'/home/zouqing/test/c/aaaa\' ret = %d\n", ret);

	ret = vpk_remove("/home/zouqing/test/c/aaaa");
	printf("vpk_remove \'/home/zouqing/test/c/aaaa\' ret = %d\n", ret);
	ret = vpk_remove("/home/zouqing/test/c/dddd");
	printf("vpk_remove \'/home/zouqing/test/c/dddd\' ret = %d\n", ret);

	vpk_enumerate("/home/zouqing/test/c", dir_print, NULL);

	printf("\r\n");
	vpk_enumerate("/home/zouqing/test/c/", dir_print, NULL);

	return 0;
}
*/
