compile
nt966x:
    ./execute.sh nt966x 1
    or
    ./premake.sh nt966x 1 true 

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


