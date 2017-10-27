#!/bin/bash
libtoolize
aclocal
autoconf
autoheader
automake --add-missing
sleep 5
#./configure --host=arm-hismall-linux
#./configure --host=mipsel-24kec-linux-uclibc
platform=
WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}"   )" && pwd   )"
echo "WORKDIR: $WORKDIR "
echo "========FINAL_PATH: $FINAL_PATH"

if [ ! "$1"x == x ]; then 
    echo "$1"
    platform=$1
else
    echo -e "\033[32m       platform unknown      \033[0m"
    exit -1
fi

function proj_path_get()
{
    if [ -d "${WORKDIR}/../$1" ]; then
        echo "${WORKDIR}/../$1"
    else
        echo -e "$1 not exist"
        exit -1
    fi
}

function find_premake()
{
    dir_premake=premake
    if [ ! -d ${dir_premake} ]; then
       
        local premake_library=$(proj_path_get premake)
        if [ -d $premake_library ]; then
            ln -s ${premake_library} ${WORKDIR}/premake
        else
            exist -1
        fi

    fi
}

DIR_PREMAKE=${WORKDIR}/premake
function nt966x_env()
{
    export INSTALL_PATH="${DIR_PREMAKE}/${platform}/install"
    export FINAL_PATH=$INSTALL_PATH

    # build environment
    export INSTALLDIR=/opt/mipsel-24kec-linux-uclibc/usr
    export PATH=$INSTALLDIR/bin:$PATH
    export TARGETMACH=mipsel-24kec-linux-uclibc
    export BUILDMACH=i686-pc-linux-gnu
    export CROSS=mipsel-24kec-linux-uclibc-
    export CC=${CROSS}gcc
    export LD=${CROSS}ld
    export AS=${CROSS}as
    export CXX=${CROSS}g++

    export GBASE_INCLUDE="/opt/mipsel-24kec-linux-uclibc/usr/mipsel-24kec-linux-uclibc/sysroot/usr/include"
    export GBASE_LIB="/opt/mipsel-24kec-linux-uclibc/usr/mipsel-24kec-linux-uclibc/sysroot/lib"
    export GOLBAL_CFLAGS="-I${GBASE_INCLUDE} -I${FINAL_PATH}/include"
    export GOLBAL_CPPFLAGS="-I${GBASE_INCLUDE} -I${FINAL_PATH}/include"
    export GOLBAL_LDFLAGS="-L${GBASE_LIB} -L${FINAL_PATH}/lib"
}

function x86_env()
{
    export INSTALL_PATH="${DIR_PREMAKE}/${platform}/install"
    export FINAL_PATH=$INSTALL_PATH

    export CC=gcc
    export CXX=g++

    export GBASE_INCLUDE="/usr/include"
    export GBASE_LIB="/usr/lib"
    export GOLBAL_CFLAGS="-I${GBASE_INCLUDE} -I${FINAL_PATH}/include"
    export GOLBAL_CPPFLAGS="-I${GBASE_INCLUDE} -I${FINAL_PATH}/include"
    export GOLBAL_LDFLAGS="-L${GBASE_LIB} -L${FINAL_PATH}/lib"
}

function project_env()
{
    if [ "${platform}" == nt966x ]; then
        nt966x_env
        echo "==============================nt966x"
            #--prefix=$WORKDIR/build_nt966x/install
    elif [ "$platform" == x86 ]; then
        x86_env
           #--prefix=$WORKDIR/build_x86/install
    else
        echo -e "\033[32m    platform unknown for configure   \033[0m"
        exit -1
    fi
}

outdir=${WORKDIR}/build_$platform
if [ -d $outdir ]; then
    rm $outdir -rf
fi

function project_clean()
{
    make clean
}

if [ $2 == 1 ]; then
    project_clean
    echo -e " \033[32m   project clean, param: $2    \033[0m"
fi

function project_config()
{
    if [ "$platform" == nt966x ]; then
        ./configure --host=mipsel-24kec-linux-uclibc \
            enable_shared=yes platform=nt966x enable_x86=no \
            enable_sqlite3=yes enable_zlog=yes \
            --prefix=$FINAL_PATH
            #--prefix=$WORKDIR/build_nt966x/install
    elif [ "$platform" == x86 ]; then
        ./configure --host=x86_64-unknown-linux-gnu \
            enable_shared=yes platform=x86 enable_x86=yes \
            enable_sqlite3=yes enable_zlog=yes \
            --prefix=$FINAL_PATH
            #--prefix=$WORKDIR/build_x86/install
    else
        echo -e "\033[32m    platform unknown for configure   \033[0m"
        exit -1
    fi
}

function project_compile()
{
    make
    make install
}

echo "============================start find_premake"
find_premake
echo "============================end find_premake"
project_env
echo "============================end project_env"

echo "============================ start project_config"
#automake --add-missing
project_config
echo "============================ end project_config"
project_compile

