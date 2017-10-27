#ifndef _NVTTYPES_H
#define _NVTTYPES_H
/*
    Copyright (c) 2015~  Novatek Microelectronics Corporation

    @file nvttypes.h

    @version

    @date
*/
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long  UINT64;     ///< Unsigned 64 bits data type
typedef signed long long    INT64;      ///< Signed 64 bits data type
typedef unsigned long       UINT32;     ///< Unsigned 32 bits data type
typedef signed long         INT32;      ///< Signed 32 bits data type
typedef unsigned short      UINT16;     ///< Unsigned 16 bits data type
typedef signed short        INT16;      ///< Signed 16 bits data type
typedef unsigned char       UINT8;      ///< Unsigned 8 bits data type
typedef signed char         INT8;       ///< Signed 8 bits data type
typedef float               FLOAT;      ///< Floating point data type
typedef double              DOUBLE;     ///< Double precision floating point data type
typedef char                CHAR;       ///< Character data type
typedef signed long         BOOL;       ///< Boolean data type
typedef INT32               ER;         ///< Error code

#define NVT_FALSE   0
#define NVT_TRUE    1

//@{
#define ENUM_DUMMY4WORD(name)   E_##name = 0x10000000
//@}

/* Error Code Define */
typedef enum {
    ER_FUNC_STOP_FAIL=-10005,
    ER_FUNC_START_FAIL=-10004,
    ER_FUNC_NOT_STOP=-10003,
    ER_FUNC_NOT_START=-10002,
    ER_INVALID_CHANNEL=-10001,
    ER_STORAGE_WRITE_FAIL=-203,
    ER_STORAGE_READ_FAIL=-202,
    ER_STORAGE_OPEN_FAIL=-201,
    ER_END_BY_INTERRUPT=-110,
    ER_RESOURCE_OUT_OF_RANGE=-109,
    ER_RESOURCE_NOT_EXIST=-108,
    ER_RESOURCE_NOT_INIT=-107,
    ER_RESOURCE_RELEASE_FAIL=-106,
    ER_ITEM_NOT_SUPPORT=-105,
    ER_GET_DATA_FAIL=-104,
    ER_TIME_OUT=-103,
    ER_MEM_ALLOC_FAIL=-102,
    ER_INVALID_INPUT_DATA=-101,
    ER_UNCLASSIFIED_ERROR=-1,
    ER_SUCCESS=0,
} NvtErrorCode;

#ifdef __cplusplus
}
#endif
/* ----------------------------------------------------------------- */
#endif /* _NVTTYPES_H  */
