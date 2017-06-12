/**
 * History:
 * ================================================================
 * 2017-03-28 qing.zou created
 *
 */
#ifndef VPK_H
#define VPK_H

#include "vpk_libc.h"
#include "vpk_util.h"
#include "vpk_logging.h"
#include "vpk_serialc.h"
#include "vpk_nvtuctrlc.h"
#include "vpk_action.h"
#include "vpk_filesys.h"
#include "vpk_queues.h"
#include "vpk_constants.h"
#include "vpk_event.h"
#include "vpk_mmap.h"
#include "vpk_md5.h"
#include "vpk_ipc.h"

TIMA_BEGIN_DELS

/**
 * Initialize the system.
 * 
 */
VPKAPI int vpk_system_init(int argc, char* argv[]);

TIMA_END_DELS

#endif/*VPK_H*/
