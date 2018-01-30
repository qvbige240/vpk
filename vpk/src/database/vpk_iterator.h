/*
 * History:
 * ================================================================
 * 2018-01-25 qing.zou created
 *
 */
#ifndef VPK_ITERATOR_H
#define VPK_ITERATOR_H

#include "vpk.h"

TIMA_BEGIN_DELS


/// the iterator mode type
typedef enum vpk_iterator_mode_t
{
	VPK_ITERATOR_MODE_FORWARD        = 1,     //!< forward iterator
	VPK_ITERATOR_MODE_REVERSE        = 2,     //!< reverse iterator
	VPK_ITERATOR_MODE_RACCESS        = 4,     //!< random access iterator
	VPK_ITERATOR_MODE_MUTABLE        = 8,     //!< mutable iterator, the item of the same iterator is mutable for removing and moving, .e.g vector, hash, ...
	VPK_ITERATOR_MODE_READONLY       = 16,    //!< readonly iterator
} vpk_iterator_mode_t;


struct vpk_iterator_t;
typedef struct vpk_iterator_t vpk_iterator_t;

struct vpk_iterator_t
{
	int			mode;
	int			step;
	void*		priv;

	int			(*size)(vpk_iterator_t* iterator);

	int			(*head)(vpk_iterator_t* iterator);

	int			(*last)(vpk_iterator_t* iterator);

	int			(*tail)(vpk_iterator_t* iterator);

	int			(*prev)(vpk_iterator_t* iterator, int index);

	int			(*next)(vpk_iterator_t* iterator, int index);

	void*		(*item)(vpk_iterator_t* iterator, int index);

	int			(*comp)(vpk_iterator_t* iterator, const void* litem, const void* ritem);

	void		(*copy)(vpk_iterator_t* iterator, int index, const void* item);

	void		(*remove)(vpk_iterator_t* iterator, int index);

	void		(*remove_range)(vpk_iterator_t* iterator, int prev, int next, int size);
};


/** the iterator mode
 * 
 * @param iterator  the iterator
 *
 * @return          the iterator mode
 */
int vpk_iterator_mode(vpk_iterator_t* iterator);

/** the iterator step
 * 
 * @param iterator  the iterator
 *
 * @return          the iterator step
 */
int vpk_iterator_step(vpk_iterator_t* iterator);

/** the iterator size
 * 
 * @param iterator  the iterator
 *
 * @return          the iterator size
 */
int vpk_iterator_size(vpk_iterator_t* iterator);

/** the iterator head
 * 
 * @param iterator  the iterator
 *
 * @return          the iterator head
 */
int vpk_iterator_head(vpk_iterator_t* iterator);

/** the iterator last
 * 
 * @param iterator  the iterator
 *
 * @return          the iterator last
 */
int vpk_iterator_last(vpk_iterator_t* iterator);

/** the iterator tail
 * 
 * @param iterator  the iterator
 *
 * @return          the iterator tail
 */
int vpk_iterator_tail(vpk_iterator_t* iterator);

/** the iterator prev
 * 
 * @param iterator  the iterator
 * @param index      the item index
 *
 * @return          the iterator prev
 */
int vpk_iterator_prev(vpk_iterator_t* iterator, int index);

/** the iterator next
 * 
 * @param iterator  the iterator
 * @param index      the item index
 *
 * @return          the iterator next
 */
int vpk_iterator_next(vpk_iterator_t* iterator, int index);

/** the iterator item
 * 
 * @param iterator  the iterator
 * @param index      the item index
 *
 * @return          the iterator item
 */
void* vpk_iterator_item(vpk_iterator_t* iterator, int index);

///** remove the iterator item
// * 
// * @param iterator  the iterator
// * @param index      the item index
// */
//void vpk_iterator_remove(vpk_iterator_t* iterator, int index);
//
///** remove the iterator items from range(prev, next)
// * 
// * @param iterator  the iterator
// * @param prev      the prev item
// * @param next      the next item
// * @param size      the removed size
// */
//void vpk_iterator_remove_range(vpk_iterator_t* iterator, int prev, int next, int size);

///** copy the iterator item
// * 
// * @param iterator  the iterator
// * @param index      the item index
// * @param item      the copied item
// */
//void vpk_iterator_copy(vpk_iterator_t* iterator, int index, vpk_cpointer_t item);
//
///** compare the iterator item
// * 
// * @param iterator  the iterator
// * @param index      the item 
// * @param item      the compared item 
// *
// * @return          =: 0, >: 1, <: -1
// */
//long vpk_iterator_comp(vpk_iterator_t* iterator, vpk_cpointer_t litem, vpk_cpointer_t ritem);


TIMA_END_DELS

#endif //VPK_ITERATOR_H
