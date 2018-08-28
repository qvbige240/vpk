#!/bin/bash
libtoolize
aclocal
autoconf
autoheader
automake --add-missing
#./configure --host=arm-hismall-linux
#./configure --host=mipsel-24kec-linux-uclibc
export PLATFORMS=
WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}"   )" && pwd   )"
echo "WORKDIR: $WORKDIR "

if [ ! "$1"x == x ]; then 
    echo "$1"
    PLATFORMS=$1
else
    #PLATFORMS=nt966x
    PLATFORMS=mips
fi
if [ "$PLATFORMS" == mips ]; then
    ./configure --host=mipsel-24kec-linux-uclibc enable_shared=yes enable_x86=no enable_sqlite3=yes \
        enable_zlog=yes enable_protocol=no --prefix=$WORKDIR/build_nt966x/install
else
    ./configure --host=x86_64-unknown-linux-gnu enable_shared=yes enable_x86=yes enable_sqlite3=yes \
        enable_zlog=yes enable_protocol=no --prefix=$WORKDIR/build_x86/install
fi
make
#make install

outdir=${WORKDIR}/build_$PLATFORMS
if [ -d $outdir ]; then
    rm $outdir -rf
fi
#mkdir $outdir
#if [ -d $outdir ]; then
#    cp ${WORKDIR}/main/test $outdir
#    cp ${WORKDIR}/vpk/*.a $outdir
#fi

make install
