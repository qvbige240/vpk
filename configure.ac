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
# project_dir=/media/zouqing/9d0165e6-5910-46c0-9389-cb9a7e7b899b/zouqing/work/project/vpk
echo "========PLATFORMS: $PLATFORMS"

includedir="$includedir/vpk"
echo "====== includedir: $includedir"
project_dir=$PWD
echo "project dir: $project_dir"
AC_SUBST(project_dir)

#if test x$enable_shared = xyes; then
#    #VPK_LIB='-lvpk'
#    #VPK_LIB='$(project_dir)/vpk/.libs/libvpk.so'
#else
#    VPK_LIB=''
#fi

if test x$enable_sqlite3 = xyes; then
    SQLITE3_LIB='libsqlite3.a'
    AC_DEFINE(USE_SQLITE3, 1, [if sqlite3 is enabled])
    DEFINE_CFLAGS="$DEFINE_CFLAGS -DUSE_SQLITE3"
else
    SQLITE3_LIB=''
fi

if test x$enable_zlog = xyes; then
    ZLOG_LIB='libzlog.a'
    AC_DEFINE(USE_ZLOG, 1, [if zlog is enabled])
    DEFINE_CFLAGS="$DEFINE_CFLAGS -DUSE_ZLOG"
else
    ZLOG_LIB=''
fi

if test x$enable_protocol = xyes; then
    TIMAPI_LIB='libtimapi.a'
    NANOMSG_LIB='libnanomsg.a'
    TIMA_PROTOCOL="LIBNANOMSG"
    AC_DEFINE(TIMA_PROTOCOL, 1, [if protocol is enabled])
    DEFINE_CFLAGS="$DEFINE_CFLAGS -DTIMA_PROTOCOL"
else
    TIMAPI_LIB=''
    NANOMSG_LIB=''
    TIMA_PROTOCOL=""
fi

CURL_LIB='libcurl.a'
JANSSON_LIB='libjansson.a'
CRYPTO_LIB='libcrypto.a'
    
#LIB_PATH='${FINAL_PATH}/lib'
DEFINE_CFLAGS="$DEFINE_CFLAGS -DUSE_TESTDEF"
echo "DEFINE_CFLAGS: $DEFINE_CFLAGS"
echo "sqlite lib: $SQLITE3_LIB"

if test x$enable_x86 = xyes; then
    PLATFORMS="_X86_"
    echo "====== x86 ======== use gcc to compile"
    GBASE_INCLUDE="/usr/include"
    GBASE_LIB="/usr/lib"
    #AM_CFLAGS="-Wall -Wextra -Wdeclaration-after-statement"
    GOLBAL_CFLAGS="-g -O0 -DDEBUG -D_X86_ $DEFINE_CFLAGS -I${GBASE_INCLUDE}"
    GOLBAL_CPPFLAGS="-g -O0 -DDEBUG -D_X86_ -I${GBASE_INCLUDE}"
    GOLBAL_LDFLAGS="-L${GBASE_LIB} -L/lib"
    AC_SUBST(GOLBAL_CFLAGS)

    LIB_PATH='$(project_dir)/x86/lib'
    AC_SUBST(LIB_PATH)
	
    #TIMAPI_LIB='$(project_dir)/x86/lib/libtimapi.a'
    #NANOMSG_LIB='$(project_dir)/x86/lib/libnanomsg.a'
    #JANSSON_LIB='$(project_dir)/x86/lib/libjansson.a'
    #CURL_LIB='$(project_dir)/x86/lib/libcurl.a'
    #CRYPTO_LIB='$(project_dir)/x86/lib/libcrypto.a'
    #SQLITE3_LIB='$(project_dir)/x86/lib/libsqlite3.a'
    #AC_SUBST(CURL_LIB)
    #AC_SUBST(JANSSON_LIB)

    STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$JANSSON_LIB $LIB_PATH/$CURL_LIB $LIB_PATH/$CRYPTO_LIB"

    if test X"$SQLITE3_LIB" != X; then
        STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$SQLITE3_LIB"
    fi

    if test X"$ZLOG_LIB" != X; then
        STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$ZLOG_LIB"
    fi

    if test X"$TIMAPI_LIB" != X; then
        STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$TIMAPI_LIB"
    fi

    if test X"$NANOMSG_LIB" != X; then
        STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$NANOMSG_LIB"
    fi

    echo "STATIC_LIBS: $STATIC_LIBS"
    AC_SUBST(STATIC_LIBS)
	
    DYNAMIC_LIBS="-lanl -ldl -lpthread -lrt -lz"
    AC_SUBST(DYNAMIC_LIBS)
    CFLAGS='-Wall $(GOLBAL_CFLAGS) -I$(project_dir)/x86/include/ -I$(project_dir)/vpk/inc -I$(project_dir)/vpk/src'
    CFLAGS+=' -I$(project_dir)/vpk/src/database'
    EXTRA_LDFLAGS='$(GOLBAL_LDFLAGS) -L$(project_dir)/vpk/.libs'
    AC_SUBST(EXTRA_LDFLAGS)
else
    PLATFORMS="_NT966X_"
    GBASE_INCLUDE="/opt/mipsel-24kec-linux-uclibc/usr/mipsel-24kec-linux-uclibc/sysroot/usr/include"
    GBASE_LIB="/opt/mipsel-24kec-linux-uclibc/usr/mipsel-24kec-linux-uclibc/sysroot/lib"
    GOLBAL_CFLAGS="-g -O0 -DDEBUG -D_NT966X_ $DEFINE_CFLAGS -I${GBASE_INCLUDE}"
    GOLBAL_CPPFLAGS="-g -O0 -DDEBUG -D_NT966X_ -I${GBASE_INCLUDE}"
    GOLBAL_LDFLAGS="-L${GBASE_LIB}"
    AC_SUBST(GOLBAL_CFLAGS)

    LIB_PATH='$(project_dir)/mips/lib'
	
    #JANSSON_LIB='$(project_dir)/mips/lib/libjansson.a'
    #CURL_LIB='$(project_dir)/mips/lib/libcurl.a'
    #CURL_LIB=''
    #SQLITE3_LIB='$(project_dir)/mips/lib/libsqlite3.a'
    #AC_SUBST(SQLITE3_LIB)
    #AC_SUBST(JANSSON_LIB)
    #AC_SUBST(CURL_LIB)
    
    #STATIC_LIBS="$STATIC_LIBS $JANSSON_LIB $CURL_LIB"
    STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$JANSSON_LIB "

    if test X"$SQLITE3_LIB" != X; then
        STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$SQLITE3_LIB"
    fi

    if test X"$ZLOG_LIB" != X; then
        STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$ZLOG_LIB"
    fi

    echo "STATIC_LIBS: $STATIC_LIBS"
	AC_SUBST(STATIC_LIBS)

	DYNAMIC_LIBS="-lcrypto -lcurl -lssl -lnvtuctrl -ldl -lpthread -lrt -lm"
	#DYNAMIC_LIBS="-lnvtuctrl -lpthread -lrt -lm"
	AC_SUBST(DYNAMIC_LIBS)
	CFLAGS='-Wall $(GOLBAL_CFLAGS) -I$(project_dir)/mips/include -I$(project_dir)/vpk/inc -I$(project_dir)/vpk/src'
	CFLAGS+=' -I$(project_dir)/vpk/src/database'
	EXTRA_LDFLAGS='-L$(project_dir)/mips/lib -L$(project_dir)/vpk/.libs'
	AC_SUBST(EXTRA_LDFLAGS)
fi
echo "platform: $PLATFORMS"
echo "platform11111: ${PLATFORMS}"
echo 'platform22222: $(PLATFORMS)'
AM_CONDITIONAL(X8666, test "${PLATFORMS}" = "_X86_")
AM_CONDITIONAL(NT966X, test "${PLATFORMS}" = "_NT966X_")
AM_CONDITIONAL(TIMA_PROTOCOL, test "${TIMA_PROTOCOL}" = "LIBNANOMSG")
#AM_CONDITIONAL(USE_SHARED, test x$enable_shared = xyes)


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
