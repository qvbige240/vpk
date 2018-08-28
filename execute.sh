#!/bin/bash
libtoolize
aclocal
autoconf
autoheader
automake --add-missing
#./configure --host=arm-hismall-linux
#./configure --host=mipsel-24kec-linux-uclibc
export platform=
export model=
WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}"   )" && pwd   )"
echo "WORKDIR: $WORKDIR "


if [ x"$1" == xnt966x_d018 ] || [ x"$1" == xnt966x_d048 ] ; then
    platform=nt966x
    model=$1
    #platform=mips
elif [ ! "$1"x == x ]; then 
    echo "$1"
    platform=$1
    model=$1
fi
if [ "$platform" == nt966x ]; then
    ./configure --host=mipsel-24kec-linux-uclibc enable_shared=yes enable_x86=no enable_sqlite3=yes \
        enable_zlog=yes enable_protocol=no --prefix=$WORKDIR/build_$platform/install
else
    ./configure --host=x86_64-unknown-linux-gnu enable_shared=yes enable_x86=yes enable_sqlite3=yes \
        enable_zlog=yes enable_protocol=no --prefix=$WORKDIR/build_$platform/install
fi
make
#make install

outdir=${WORKDIR}/build_$platform
if [ -d $outdir ]; then
    rm $outdir -rf
fi
#mkdir $outdir
#if [ -d $outdir ]; then
#    cp ${WORKDIR}/main/test $outdir
#    cp ${WORKDIR}/vpk/*.a $outdir
#fi

make install
