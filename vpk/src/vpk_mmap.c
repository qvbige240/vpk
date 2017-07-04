/**
 * History:
 * ================================================================
 * 2017-04-16 qing.zou created
 *
 */
#include <sys/stat.h>

#include "vpk_mmap.h"

struct _VpkMmap
{
	FILE*  fp;
	void*  data;
	size_t length;
};


int vpk_mmap_exist(const char* filename)
{
	//struct stat_t st = {0};
	struct stat st = {0};
	return_val_if_fail(filename != NULL, 0);

	//(access(path, 0) == 0)
	return (stat(filename, &st) == 0);
}

int vpk_mmap_save(const char* filename, void* data, size_t size)
{
	FILE* fp = 0;
	size_t ret = 0;
	return_val_if_fail(filename != NULL && data != NULL, -1);

	fp = fopen(filename, "w+");
	if (fp != NULL)
	{
		ret = fwrite(data, 1, size, fp);
		fclose(fp);
	}
	if (ret != size)
		printf("fwrite size(%d != %d) incorrect!", ret, size);

	return ret;
}

VpkMmap* vpk_mmap_create(const char* filename, size_t offset, size_t size)
{
	VpkMmap* thiz = NULL;
	/*struct stat st = {0};

	return_val_if_fail(filename != NULL, NULL);
	return_val_if_fail(stat(filename, &st) == 0, NULL);
	return_val_if_fail(offset < st.st_size, NULL);

	size = (offset + size) <= st.st_size ? size : st.st_size;*/
	int file_size = 0;
	FILE* fp = 0;

	return_val_if_fail(filename != NULL, NULL);
	fp = fopen(filename, "rb");
	return_val_if_fail(fp != NULL, NULL);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return_val_if_fail(offset < file_size, NULL);

	size = (offset + size) <= file_size ? size : file_size;

	thiz = TIMA_MALLOC(sizeof(VpkMmap));
	return_val_if_fail(thiz != NULL, NULL);

	//thiz->fp = fopen(filename, "rb");
	thiz->fp = fp;
	if(thiz->fp != NULL)
	{
		thiz->length = size;
		thiz->data = TIMA_MALLOC(size + 1);
		if(thiz->data == NULL)
		{
			fclose(thiz->fp);
			memset(thiz, 0x00, sizeof(*thiz));
			TIMA_FREE(thiz);
			thiz = NULL;
			return NULL;
		}

		fseek(thiz->fp, offset, SEEK_SET);
		//fread(thiz->data, thiz->length, 1, thiz->fp);
		fread(thiz->data, 1, thiz->length, thiz->fp);
		*((char *)thiz->data + thiz->length) = '\0';
		fclose(thiz->fp);
	}
	return thiz;
}

void*	  vpk_mmap_data(VpkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);
	
	return thiz->data;
}

size_t    vpk_mmap_length(VpkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->length;
}

void	  vpk_mmap_destroy(VpkMmap* thiz)
{
	if (thiz != NULL)
	{
		if (thiz->data)
			TIMA_FREE(thiz->data);
		memset(thiz, 0x00, sizeof(*thiz));
		TIMA_FREE(thiz);
	}

	return;
}
