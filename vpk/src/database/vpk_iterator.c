/*
 * History:
 * ================================================================
 * 2018-01-25 qing.zou created
 *
 */

#include "vpk_iterator.h"


int vpk_iterator_mode(vpk_iterator_t* iterator)
{
	return_val_if_fail(iterator, -1);

	return iterator->mode;
}

int vpk_iterator_step(vpk_iterator_t* iterator)
{
	return_val_if_fail(iterator, -1);

	return iterator->step;
}

int vpk_iterator_size(vpk_iterator_t* iterator)
{
	return_val_if_fail(iterator && iterator->size, -1);

	return iterator->size(iterator);
}

int vpk_iterator_head(vpk_iterator_t* iterator)
{
	return_val_if_fail(iterator && iterator->head, -1);

	return iterator->head(iterator);
}

int vpk_iterator_last(vpk_iterator_t* iterator)
{
	return_val_if_fail(iterator && iterator->last, -1);

	return iterator->last(iterator);
}

int vpk_iterator_tail(vpk_iterator_t* iterator)
{
	return_val_if_fail(iterator && iterator->tail, -1);

	return iterator->tail(iterator);
}

int vpk_iterator_prev(vpk_iterator_t* iterator, int index)
{
	return_val_if_fail(iterator && iterator->prev, -1);

	return iterator->prev(iterator, index);
}

int vpk_iterator_next(vpk_iterator_t* iterator, int index)
{
	return_val_if_fail(iterator && iterator->next, -1);

	return iterator->next(iterator, index);
}

void* vpk_iterator_item(vpk_iterator_t* iterator, int index)
{
	return_val_if_fail(iterator && iterator->item, NULL);

	return iterator->item(iterator, index);
}

//void vpk_iterator_remove(vpk_iterator_t* iterator, int index)
//{
//	return_val_if_fail(iterator && iterator->remove);
//
//	return iterator->remove(iterator, index);
//}
//
//void vpk_iterator_remove_range(vpk_iterator_t* iterator, int prev, int next, int size);
