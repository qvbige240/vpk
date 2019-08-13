platforms are as follow:
    x86, nt966x, nt966x_d048
compile
nt966x:
    ./execute.sh nt966x 1
    or
    ./premake.sh nt966x 1 true 
    ./premake.sh nt966x_d048 1 true 

x86:
    ./execute.sh x86 1
    or
    ./premake.sh x86 1 true


# build without clean project, just make using 
# environment with previous configuration and don't automake
./execute.sh x86 
or 
./execute.sh nt966x


# build out path
build out path: ${FINAL_PATH}/xxxplatform/install/


sample use
$ ./test -d mqsend -k 2001
$ ./test -h

Then link your C program against sample:
 export FINAL_PATH=/home/zouqing/work/carnet/linux/auto/premake/x86/install
 export GOLBAL_CFLAGS="-I/usr/include -I$FINAL_PATH/include"
 export GOLBAL_CPPFLAGS="-I/usr/include -I$FINAL_PATH/include"
 export GOLBAL_LDFLAGS="-L/usr/lib -L$FINAL_PATH/lib"
# export GOLBAL_CFLAGS="-I/usr/include -I/home/zouqing/work/carnet/linux/auto/premake/x86/install/include"
# export GOLBAL_CPPFLAGS="-I/usr/include -I/home/zouqing/work/carnet/linux/auto/premake/x86/install/include"
# export GOLBAL_LDFLAGS="-L/usr/lib -L/home/zouqing/work/carnet/linux/auto/premake/x86/install/lib"

 libtoolize
 aclocal
 autoconf
 autoheader
 automake --add-missing

$ make distclean
$ mkdir build
$ cd build
$ ../configure --prefix=$PWD/install platform=x86 --enable-sqlite3 --enable-protocol

export FINAL_PATH=xxxx
$ ../configure --prefix=$PWD/install platform=nt966x --enable-sqlite3

