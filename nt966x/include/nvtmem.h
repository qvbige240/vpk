/**
    NvtMem API export

    Sample module detailed description.
    @file nvtmem.h
    @ingroup NVTMEM
    @note Nothing (or anything need to be mentioned).
    Copyright Novatek Microelectronics Corp. 2014. All rights reserved.
*/
#ifndef _NVTMEM_H
#define _NVTMEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
    Change a phyical address to non-cached address

    @param addr: phyical address.

    @return
        - @b 0: this phyical address is not in region of Memory Pool.
        - @b otherwise: non-cached address.
*/
extern unsigned int NvtMem_GetNonCacheAddr(unsigned int addr);

/**
    Change a non-cached address to phyical address address

    @param addr: non-cached address.

    @return
        - @b 0: this non-cached address is not in region of Memory Pool.
        - @b otherwise: non-cached address.
*/
extern unsigned int NvtMem_GetPhyAddr(unsigned int addr);

#ifdef __cplusplus
}
#endif
/* ----------------------------------------------------------------- */
#endif /* _PSTORE_H  */
