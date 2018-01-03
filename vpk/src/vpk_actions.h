/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_ACTIONS_H
#define VPK_ACTIONS_H

#include "vpk_typedef.h"
#include "vpk_action.h"

TIMA_BEGIN_DELS

typedef struct _ActionInfo
{
	VpkActionType	type;

	char*			name;
	char*			action;
} ActionInfo;

extern const ActionInfo vpk_action_tables[];
extern const int vpk_action_tables_size;

TIMA_END_DELS

#endif //VPK_ACTIONS_H
