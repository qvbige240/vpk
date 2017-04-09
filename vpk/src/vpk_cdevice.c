/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */

#include "vpk_logging.h"

#include "vpk_cdevice.h"


int vpk_register_cdevice(cdevice_register_t* thiz, const char* standard, const void* ops, const char* name)
{
	cdevice_item_t* item = NULL;
	return_val_if_fail(thiz != NULL && standard != NULL && ops != NULL, -1);

	if (thiz->count >= _countof(thiz->items))
	{
		LOG_E("too many %s device have been registed!", standard);
		return -1;
	}

	item = &thiz->items[thiz->count++];
	//strncpy(item->standard, standard, strlen(standard));
	//strlcpy(item->standard, standard, _countof(item->standard));
	strncpy(item->standard, standard, sizeof(item->standard));
	item->ops = ops;

	return 0;
}

const void* vpk_find_cdevice(cdevice_register_t* thiz, const char* standard)
{
	unsigned int i = 0;
	return_val_if_fail(thiz != NULL && standard != NULL, NULL);

	for (i = 0; i < _countof(thiz->items); i++)
	{
		if (strncmp(standard, thiz->items[i].standard, _countof(thiz->items[i].standard)) == 0) //...
		{
			return thiz->items->ops;
		}
	}
	return NULL;
}
