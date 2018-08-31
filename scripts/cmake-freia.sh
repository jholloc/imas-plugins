#!/bin/bash

module purge
module load imas-modules
module load imas
module load idl/08.3
module load java/1.8

export HDF5_USE_SHLIB=yes
export BOOST_ROOT=/usr/local/depot/boost-1.60

cmake3 -Bbuild_freia -H. -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=MAST \
    -DCMAKE_INSTALL_PREFIX=${HOME}/freia \
    -DSWIG_EXECUTABLE=/home/jholloc/freia/bin/swig \
    $*
