/*
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
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
		{
			printf("mkdir \'%s\' error\n", path);
			return -1;
		}
	}
	else if (!S_ISDIR(st.st_mode))
	{
		printf("mkdir: cannot create directory \'%s\': File exists\n", path);
		return -1;
	}

	return 0;
}

VPKAPI int vpk_mkdir_mult(const char* path)
{
	int len, i;
	char tmp[FILENAME_MAX] = {0};

	if (path == NULL)
		return -1;

	len = strlen(path);
	if (len >= FILENAME_MAX-1)
		return -1;

	strncpy(tmp, path, len);
	if (tmp[len-1] != '/')
	{
		tmp[len] = '/';
		tmp[len+1] = '\0';
	}

	for (i = 0; i < len; i++)
	{
		if (tmp[i] == '/' && i > 0)
		{
			tmp[i] = '\0';
			if (access(tmp, F_OK) != 0)		/* not exists */
			{
				if (mkdir(tmp, 0777) != 0)
				{
					printf("error: mkdir \'%s\'!\n", tmp);
					return -1;
				}
				printf("mkdir \'%s\'\n", tmp);
			}
			else if (!vpk_isdir(tmp))
			{
				printf("mkdir: cannot create directory \'%s\': File exists\n", tmp);
				return -1;
			}
			tmp[i] = '/';
		}
	}

	return 0;
}

int vpk_create_file(const char *filename)
{
	if (creat(filename, 0755) < 0) {

		printf("creat \'%s\' error\n", filename);
		return -1;
	}
	return 0;
}

int vpk_pathname_get(const char *full, char *path)
{
	const char *pos = strrchr(full, '/');
	if (pos == NULL)
	{
		if (vpk_isdir(full))
		{
			int len = strlen(full);
			strcpy(path, full);
			path[len] = '/';
			path[len+1] = '\0';
		}
		else
		{
			strcpy(path, "./");
		}
	}
	else 
	{
		strncpy(path, full, pos+1-full);
		path[pos+1-full] = '\0';
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

VPKAPI int vpk_rename(const char* oldname, const char* newname)
{
	if (!oldname || !newname)
		printf("%s oldname or newname error\n", __FUNCTION__);

	if (rename(oldname, newname) < 0) {
		printf("%s file rename error: %s\n", __FUNCTION__, oldname);
		return -1;
	}
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
int process_path_name_get(pid_t pid, char *path_name)
{
	FILE *fp;
	int ret = -1;
	char cmd[255] = {0};
	sprintf(cmd, "readlink /proc/%d/exe", pid);

	if ( (fp = popen(cmd, "r")) != NULL)
	{
		if (fgets(path_name, 254, fp) != NULL)
		{
			printf("process_path_name_get: %s \n", path_name);
			ret = 0;
		}
	} else {
		printf("error: popen at process_name_get()!");
	}

	pclose(fp);

	return ret;
}

int process_path_name_get(pid_t pid, char *path_name)
{
    FILE *fp;
    int ret = -1;
    char cmd[255] = {0};
    sprintf(cmd, "readlink /proc/%d/exe", pid);

    if ( (fp = popen(cmd, "r")) != NULL)
    {
        if (fgets(path_name, 254, fp) != NULL)
        {
            printf("process_path_name_get: %s \n", path_name);
            ret = 0;
        }
    }
    else {
        printf("error: popen at process_name_get()!");
    }

    pclose(fp);

    return ret;
}
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
