


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define os_fseeki64  fseeko
#define os_ftelli64  ftello
#define os_fopen     fopen

#define bfree        free
