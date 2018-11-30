#!/bin/bash

module purge
module use /common/ETS/modules
module load imas/3.20.0/ual/3.8.5
# blitz needed to use imas pkg-config
module load blitz
module load blitz-devel

export HDF5_USE_SHLIB=yes
export BOOST_ROOT=/usr/local/depot/boost-1.60

UDA_HOME=/common/projects/UDA/2.2.3
export PKG_CONFIG_PATH=$UDA_HOME/lib/pkgconfig:$PKG_CONFIG_PATH

cmake3 -Bbuild_freia -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
    -DLIBSSH_ROOT=/common/projects/UDA/libssh \
    -DBUILD_PLUGINS="exp2imas;imasdd;source;mast_imas" \
    $*
