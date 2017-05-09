#!/bin/bash
#libtoolize
aclocal
autoconf
autoheader
automake --add-missing
#./configure --host=arm-hismall-linux
#./configure --host=mipsel-24kec-linux-uclibc
platform=
if [ ! "$1"x == x ]; then 
    echo "$1"
    platform=$1
else
    #platform=nt966x
    platform=mips
fi
if [ "$platform" == mips ]; then
    ./configure --host=mipsel-24kec-linux-uclibc enable_x86=no
else
    ./configure --host=x86_64-unknown-linux-gnu enable_x86=yes
fi
make
