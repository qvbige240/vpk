#ifndef _OPERATION_TYPEDEF_H_
#define _OPERATION_TYPEDEF_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>

#define return_if_fail(p, ret)                                            \
  if (!(p)) {                                                             \
    printf("%s:%d Error: " #p " failed.\n", __FILE__, __LINE__);          \
    fprintf(stderr, "%s:%d Error: " #p " failed.\n", __FILE__, __LINE__); \
    return;                                                               \
  }
#define return_val_if_fail(p, ret)                                        \
  if (!(p)) {                                                             \
    printf("%s:%d Error: " #p " failed.\n", __FILE__, __LINE__);          \
    fprintf(stderr, "%s:%d Error: " #p " failed.\n", __FILE__, __LINE__); \
    return ret;                                                           \
  }

// wrong
// printf("WARN  (%s:%s:%d) - %s\n", __FILE__, __func__, __LINE__, format, ##args)

// #define LOG_PRINT(format, args...) printf(format, ##args)

#define OPERA_LOGF(format, args...)                                       \
  printf("FATAL (%s:%s:%d) - " format "\n", __FILE__, __func__, __LINE__, ##args)
#define OPERA_LOGE(format, args...)                                       \
  printf("ERROR (%s:%s:%d) - " format "\n", __FILE__, __func__, __LINE__, ##args)
#define OPERA_LOGW(format, args...)                                       \
  printf("WARN  (%s:%s:%d) - " format "\n", __FILE__, __func__, __LINE__, ##args)
#define OPERA_LOGI(format, args...)                                       \
  printf("INFO  (%s:%s:%d) - " format "\n", __FILE__, __func__, __LINE__, ##args)
#define OPERA_LOGD(format, args...)                                       \
  printf("DEBUG (%s:%s:%d) - " format "\n", __FILE__, __func__, __LINE__, ##args)

#define vz_freep(p) \
  if (p) {          \
    delete p;       \
    p = NULL;       \
  }                 \
  (void)0
// Please use the freepa(T[]) to free an array, otherwise the behavior is undefined.
#define vz_freepa(pa) \
  if (pa) {           \
    delete[] pa;      \
    pa = NULL;        \
  }                   \
  (void)0

#endif /* _OPERATION_TYPEDEF_H_ */
