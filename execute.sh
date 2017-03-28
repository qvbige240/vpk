#!/bin/bash
aclocal
autoconf
autoheader
automake --add-missing
#./configure --host=arm-hismall-linux
./configure --host=mipsel-24kec-linux-uclibc
make
