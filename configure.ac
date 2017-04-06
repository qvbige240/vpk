#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([test], 1.0)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])

# Checks for programs.
AC_PROG_CC
#AC_PROG_RANLIB
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lvpk':
AC_CHECK_LIB([vpk], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h stdint.h stdlib.h string.h sys/time.h termios.h unistd.h])
project_dir=/media/zouqing/9d0165e6-5910-46c0-9389-cb9a7e7b899b/zouqing/work/project/vpk
AC_SUBST(project_dir)

if test x$enable_x86 = xyes; then
    GBASE_INCLUDE="/usr/include"
    GBASE_LIB="/usr/lib"
    echo "use gcc to compile"
    #AM_CFLAGS="-Wall -Wextra -Wdeclaration-after-statement"
	GOLBAL_CFLAGS="-DDEBUG -I${GBASE_INCLUDE}"
	GOLBAL_CPPFLAGS="-DDEBUG -I${GBASE_INCLUDE}"
	GOLBAL_LDFLAGS="-L${GBASE_LIB} -L/lib"
	AC_SUBST(GOLBAL_CFLAGS)
	
	JANSSON_LIB='$(project_dir)/nvt/lib/x86/libjansson.a'
	AC_SUBST(JANSSON_LIB)
	
	DYNAMIC_LIBS=-lpthread -lm -lc
	AC_SUBST(DYNAMIC_LIBS)
	CFLAGS='-Wall $(GOLBAL_CFLAGS) -I$(project_dir)/nvt/include/x86 -I$(project_dir)/vpk/inc -I$(project_dir)/vpk/src'
	EXTRA_LDFLAGS='$(GOLBAL_LDFLAGS)'
	AC_SUBST(EXTRA_LDFLAGS)
else
	GBASE_INCLUDE="/opt/mipsel-24kec-linux-uclibc/mipsel-24kec-linux-uclibc/sysroot/usr/include"
	GBASE_LIB="/opt/mipsel-24kec-linux-uclibc/mipsel-24kec-linux-uclibc/sysroot/lib"
	GOLBAL_CFLAGS="-DDEBUG -I${GBASE_INCLUDE}"
	GOLBAL_CPPFLAGS="-DDEBUG -I${GBASE_INCLUDE}"
	GOLBAL_LDFLAGS="-L${GBASE_LIB}"
	AC_SUBST(GOLBAL_CFLAGS)
	
	JANSSON_LIB='$(project_dir)/nvt/lib/mips/libjansson.a'
	AC_SUBST(JANSSON_LIB)
	DYNAMIC_LIBS="-lnvtuctrl -lpthread -lm"
	AC_SUBST(DYNAMIC_LIBS)
	EXTRA_LDFLAGS='-L$(project_dir)/nvt/lib'
	AC_SUBST(EXTRA_LDFLAGS)
	CFLAGS='-Wall $(GOLBAL_CFLAGS) -I$(project_dir)/nvt/include -I$(project_dir)/vpk/inc -I$(project_dir)/vpk/src'
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([bzero memset select strstr])

AC_CONFIG_FILES([Makefile
                 main/Makefile
                 vpk/Makefile])

AC_OUTPUT
