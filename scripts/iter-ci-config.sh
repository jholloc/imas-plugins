# Set up environment for compilation
. /usr/share/Modules/init/sh
module use /work/imas/etc/modulefiles

module purge

module load cmake/3.0.2
module load Boost/1.66.0-foss-2018a
module load openssl/1.0.2g
module load MDSplus/7.7.12-intel-2018a

UDA_HOME=/home/ITER/hollocj/uda-install

export LIBSSH_DIR=$HOME/libssh
export PKG_CONFIG_PATH=$UDA_HOME/lib/pkgconfig:$HOME/IMAS/access-layer/lib/pkgconfig:$PKG_CONFIG_PATH

CC=gcc CXX=g++ cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME
