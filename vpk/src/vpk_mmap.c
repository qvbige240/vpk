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
	fp = fopen(filename, "r");
	return_val_if_fail(fp != NULL, NULL);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return_val_if_fail(offset < file_size, NULL);

	size = (offset + size) <= file_size ? size : file_size;

	thiz = TIMA_MALLOC(sizeof(VpkMmap));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->fp = fopen(filename, "rb");
	if(thiz->fp != NULL)
	{
		thiz->length = size;
		thiz->data = TIMA_MALLOC(size + 1);
		fseek(thiz->fp, offset, SEEK_SET);
		fread(thiz->data, thiz->length, 1, thiz->fp);
		*((char *)thiz->data + thiz->length) = '\0';
		fclose(thiz->fp);
	}

	if(thiz->data == NULL)
	{
		memset(thiz, 0x00, sizeof(*thiz));
		TIMA_FREE(thiz);
		thiz = NULL;
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
