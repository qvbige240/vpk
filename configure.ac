#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([vpk], 1.0, [bugfixed])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])

dnl Save the CFLAGS to see whether they were passed in or generated
global_CFLAGS="$CFLAGS"

AC_SUBST(LIBVPK_VERSION_INFO, [1:1:0])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lvpk':
AC_CHECK_LIB([vpk], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h stdint.h stdlib.h string.h sys/time.h termios.h unistd.h])

if test x$FINAL_PATH = x; then
    AC_MSG_ERROR([
*** dependency libraries env path missing: export \$FINAL_PATH=    ***
    ])
fi

#export FINAL_PATH=/home/zouqing/work/carnet/linux/auto/premake/x86/install
#export GOLBAL_CFLAGS="-I/usr/include -I/home/zouqing/work/carnet/linux/auto/premake/x86/install/include"
#export GOLBAL_CPPFLAGS="-I/usr/include -I/home/zouqing/work/carnet/linux/auto/premake/x86/install/include"
#export GOLBAL_LDFLAGS="-L/usr/lib -L/home/zouqing/work/carnet/linux/auto/premake/x86/install/lib"
#export platform=x86

INCLUDE="-I$srcdir/include"
if test x$srcdir != x.; then
    INCLUDE="-Iinclude $INCLUDE"
fi

BASE_CFLAGS=""
BASE_LDFLAGS=""

BUILD_CFLAGS="$CFLAGS $CPPFLAGS -DUSING_GENERATED_CONFIG_H"
echo "CFLAGS: $CFLAGS"
# The default optimization for RL is -O3
if test "x$global_CFLAGS" = x; then
    BUILD_CFLAGS=`echo $BUILD_CFLAGS | sed 's/-O2/-O3/'`
fi

WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}"   )" && pwd   )"

LIB_PATH='${FINAL_PATH}/lib'
library_dir="$FINAL_PATH/lib"
echo "srcdir: $srcdir"
echo "exec_prefix: $exec_prefix"
echo "libdir: $libdir"
echo 'top_srcdir: $(top_srcdir)'
echo "top_srcdir: $top_srcdir"
echo "library_dir: $library_dir"

echo "======WORKDIR: $WORKDIR "
echo "======DIR_PREMAKE: $DIR_PREMAKE"
echo "========BUILD_PATH: $BUILD_PATH "
echo "========FINAL_PATH: $FINAL_PATH"
echo "========GBASE_INCLUDE: $GBASE_INCLUDE"
echo "========GBASE_LIB: $GBASE_LIB"
echo "========GOLBAL_CFLAGS: $GOLBAL_CFLAGS"
echo "========GOLBAL_CPPFLAGS: $GOLBAL_CPPFLAGS"
echo "========GOLBAL_LDFLAGS: $GOLBAL_LDFLAGS"
echo "========platform: $platform"

includedir="$includedir/vpk"
echo "====== includedir: $includedir"

## zlog config
AC_ARG_ENABLE(zlog,
    AS_HELP_STRING([--enable-zlog], [support zlog [[default=yes]]]),,
    enable_zlog="yes")
if test x$enable_zlog = xyes; then    
    AC_DEFINE(HAVE_ZLOG, 1, [if zlog is enabled])
    EXTRA_CFLAGS="$EXTRA_CFLAGS -DUSE_ZLOG"
    ZLOG_LIB='libzlog.a'
    STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$ZLOG_LIB"
else
    ZLOG_LIB=''
fi

## protocol config
AC_ARG_ENABLE(protocol,
    AS_HELP_STRING([--disable-protocol], [support protocol [[default=no]]]),,
    enable_protocol="no")
if test x$enable_protocol = xyes; then    
    AC_DEFINE(TIMA_PROTOCOL, 1, [if protocol is enabled])
    BUILD_CFLAGS="$BUILD_CFLAGS -DTIMA_PROTOCOL"
    TIMAPI_LIB='libtimapi.a'
    NANOMSG_LIB='libnanomsg.a'
    TIMA_PROTOCOL="LIBNANOMSG"
    STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$TIMAPI_LIB"
    STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$NANOMSG_LIB"
else
    TIMAPI_LIB=''
    NANOMSG_LIB=''
    TIMA_PROTOCOL=""
fi

## sqlite3 config
CheckSqlite3()
{
    AC_ARG_ENABLE(sqlite3,
        AS_HELP_STRING([--disable-sqlite3], [support sqlite3 [[default=no]]]),,
        enable_sqlite3="no")
    #if test x$enable_nmon = xyes -a x$enable_alsa = xyes; then
    if test x$enable_sqlite3 = xyes; then
        AC_DEFINE(HAVE_SQLITE3, 1, [ if sqlite3 is enabled ])
        SQLITE3_CFLAGS="-DUSE_SQLITE3 "
        SQLITE3_LIBS="$LIB_PATH/libsqlite3.a"
        AC_SUBST(SQLITE3_CFLAGS)
        AC_SUBST(SQLITE3_LIBS)
    fi

    AM_CONDITIONAL(HAVE_SQLITE3, [test "$enable_sqlite3" = "yes"])
}

CheckSqlite3
echo "sqlite lib: $SQLITE3_LIBS"

CURL_LIB='libcurl.a'
JANSSON_LIB='libjansson.a'
CRYPTO_LIB='libcrypto.a'
#STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$JANSSON_LIB $LIB_PATH/$CURL_LIB $LIB_PATH/$CRYPTO_LIB"

if test x$platform = xx86; then
    PLATFORMS="_X86_"
    echo "====== x86 ======== use gcc to compile"

    DYNAMIC_LIBS="-lanl"

elif test x$platform = xnt966x; then
    PLATFORMS="_NT966X_"
    echo '=========  nt966x ======='
	
    #DYNAMIC_LIBS="-lcrypto -lcurl -lssl -lnvtuctrl -ldl -lpthread -lrt -lm"
    DYNAMIC_LIBS="-lnvtuctrl"
    #AC_SUBST(DYNAMIC_LIBS)

elif test x$platform = xnt966x_d048; then
    PLATFORMS="_GENERIC_MIPS_"
    echo '=========  nt966x_d048  ======='

    #VPK_CFLAGS="-fPIC -g -O0 -DDEBUG -D$PLATFORMS $DEFINE_CFLAGS ${GOLBAL_CFLAGS} "
    #AC_SUBST(VPK_CFLAGS)

    #STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$JANSSON_LIB "
    #AC_SUBST(STATIC_LIBS)
    #echo "STATIC_LIBS: $STATIC_LIBS"

    #DYNAMIC_LIBS="-lcrypto -lcurl -lssl -ldl -lpthread -lrt -lm"
    #AC_SUBST(DYNAMIC_LIBS)

    #CFLAGS='-Wall ${VPK_CFLAGS} -I$(project_dir)/vpk/inc -I$(project_dir)/vpk/src'
    #CFLAGS+=' -I$(project_dir)/vpk/src/database'
    #echo "CFLAGS: $CFLAGS"

elif test x$platform = xubuntu; then
    PLATFORMS="_GENERIC_X86_"   #PLATFORMS="_UBUNTU_"
    echo '=========  ubuntu  ======='

    DYNAMIC_LIBS="-lanl"

elif test x$platform = xcentos; then
    PLATFORMS="_GENERIC_X86_"   #PLATFORMS="_CENTOS_"
    echo '=========  centos  ======='

    DYNAMIC_LIBS="-lanl"
else
    echo '=========  unknown platform  ========'
fi

STATIC_LIBS="$STATIC_LIBS $LIB_PATH/$JANSSON_LIB"
AC_SUBST(STATIC_LIBS)

DYNAMIC_LIBS="$DYNAMIC_LIBS -lcurl -lssl -lcrypto -ldl -lpthread -lrt -lm"
AC_SUBST(DYNAMIC_LIBS)
echo "STATIC_LIBS: $STATIC_LIBS"
echo "DYNAMIC_LIBS: $DYNAMIC_LIBS"

EXTRA_LDFLAGS='$(GOLBAL_LDFLAGS)'
#AC_SUBST(EXTRA_LDFLAGS)
LDFLAGS="$EXTRA_LDFLAGS"

EXTRA_CFLAGS="-fPIC -g -O0 -DDEBUG -D$PLATFORMS $BUILD_CFLAGS ${GOLBAL_CFLAGS} "
#AC_SUBST(EXTRA_CFLAGS)
# if use the AC_SUBST, could define CFLAGS='-Wall $(EXTRA_CFLAGS)'
CFLAGS="-Wall ${EXTRA_CFLAGS}"
echo "BUILD_CFLAGS: $BUILD_CFLAGS"
echo "CFLAGS: $CFLAGS"
echo "LDFLAGS: $LDFLAGS"

echo "platform00000: $PLATFORMS"
echo "platform11111: ${PLATFORMS}"
echo 'platform22222: $(PLATFORMS)'
AM_CONDITIONAL(PF_X86, test "${PLATFORMS}" = "_X86_")
AM_CONDITIONAL(PF_NT966X, test "${PLATFORMS}" = "_NT966X_")
AM_CONDITIONAL(PF_GENERIC_MIPS, test "${PLATFORMS}" = "_GENERIC_MIPS_")
#AM_CONDITIONAL(PF_GENERIC_X86, test "${PLATFORMS}" = "_UBUNTU_" || test "${PLATFORMS}" = "_CENTOS_")
AM_CONDITIONAL(PF_GENERIC_X86, test "${PLATFORMS}" = "_GENERIC_X86_")
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
