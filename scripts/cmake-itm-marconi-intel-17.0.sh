#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module load cineca
module load imasenv/3.33.0/gcc  
module load libssh
module load cmake/3.5.2
module load itm-intel/17.0
module load boost/1.61.0--intelmpi--2017--binary

module unload uda
module load uda/2.5.0-alpha/intel/17.0


export CC=icc
export CXX=icpc

BUILD_DIR ?=build-intel-17

cmake -B"$BUILD_DIR" -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLibSSH_ROOT="$LIBSSH_ROOT" \
    -DCMAKE_INSTALL_PREFIX="$UDA_HOME" \
    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_partial\;imas_remote "$@"
