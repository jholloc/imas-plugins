#!/bin/bash

eval $(tclsh /work/imas/opt/modules-tcl/modulecmd.tcl $(basename $SHELL) autoinit)

module purge
module use /work/imas/etc/modules/all
module load cmake/3.0.2
module load libxml2/2.9.2
module load boost/1.58
module load swig/3.0.5
module load openssl/1.0.2g
module load hdf5/1.8.13
module load MDSplus/7.7.12-intel-2018a

UDA_HOME=/home/ITER/hollocj/uda-install

export LIBSSH_DIR=$HOME/libssh
export PKG_CONFIG_PATH=$UDA_HOME/lib/pkgconfig:$HOME/IMAS/access-layer/lib/pkgconfig:$PKG_CONFIG_PATH
export BOOST_ROOT=/work/imas/opt/boost/1.58
export OPENSSL_ROOT_DIR=/work/imas/opt/openssl/1.0.2g
export HDF5_ROOT=$H5DIR

CC=gcc CXX=g++ cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME
