#ifndef _VZ_TYPEDEF_H_
#define _VZ_TYPEDEF_H_

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <limits.h>

#define VZ_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define VZ_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define VZ_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define VZ_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define VZ_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
// #define VZ_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define VZ_BREAK_IF(cond)           if(cond) break


#endif // _VZ_TYPEDEF_H_
