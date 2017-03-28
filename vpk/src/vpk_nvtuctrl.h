/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#ifndef VPK_NVTUCTRL_H
#define VPK_NVTUCTRL_H

#include "vpk_typedef.h"
#include "vpk_session.h"
//#include "vpk_serialc.h"

TIMA_BEGIN_DELS

// #define extends_serial()		\
// 	extends_session();			\
// 	uint32_t baud_speed;		\
// 	uint32_t data_bits;			\
// 	uint8_t  parity_bit;		\
// 	uint32_t stop_bit
#define extends_nvtuctrl()		\
	extends_session();			\
	void*	 data_buff;				\
	uint32_t total_size;

struct vpk_nvtuctrl_t
{
	extends_nvtuctrl();
};

int register_nvtuctrl_device(const char* standard, const session_ops* sops);

TIMA_END_DELS

#endif //VPK_NVTUCTRL_H
