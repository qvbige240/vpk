#!/bin/bash
#libtoolize
#aclocal
#autoconf
#autoheader
#automake --add-missing
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

outdir=${WORKDIR}/build_$platform
if [ -d $outdir ]; then
    rm $outdir -rf
fi

function project_clean()
{
    make clean
}

if [ $2 ]; then
    project_clean
    echo -e " \033[32m   project clean, param: $2    \033[0m"
fi

function project_config()
{
    if [ "$platform" == nt966x ]; then
        ./configure --host=mipsel-24kec-linux-uclibc \
            platform=nt966x --enable-sqlite3 \
            --prefix=$FINAL_PATH
            #--prefix=$WORKDIR/build_nt966x/install
    elif [ "$platform" == x86 ]; then
        ./configure --host=x86_64-unknown-linux-gnu \
            platform=x86 --enable-sqlite3 --enable-protocol \
            --prefix=$FINAL_PATH
            #--prefix=$WORKDIR/build_x86/install
    elif [ "$platform" == nt966x_d048 ]; then
        ./configure --host=mipsel-24kec-linux-uclibc \
            platform=nt966x_d048 --enable-sqlite3 \
            --prefix=$FINAL_PATH
    elif [ "$platform" == ubuntu ]; then
        ./configure --host=x86_64-unknown-linux-gnu \
            --prefix=$FINAL_PATH platform=ubuntu
    elif [ "$platform" == centos ]; then
        ./configure --prefix=$FINAL_PATH platform=centos
    else
            #enable_shared=yes platform=nt966x_d048 enable_x86=no
            #enable_sqlite3=yes enable_zlog=yes
        echo -e "\033[32m    platform unknown for configure   \033[0m"
        exit -1
    fi
}

function project_compile()
{
    make
    make install
}

project_config
project_compile

