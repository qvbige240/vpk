#!/bin/bash
#export PATH=$PATH:/opt/mipsel-24kec-linux-uclibc/usr/bin
source env.profile
make clean
bash execute.sh mips
make clean
bash execute.sh x86

