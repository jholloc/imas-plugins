#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module load cineca
module load imasenv/3.33.0/gcc
module load libssh

module unload uda
module load itm-boost/1.78.0/gcc/4.8
module load uda/2.5.0/gcc/7.3.0


export CC=gcc
export CXX=g++
export BOOST_ROOT=/afs/eufus.eu/user/g/g2jhollo/boost_1_62_0

AL_VERSION=$(echo $IMAS_PREFIX | rev | cut -d '/' -f 1 | rev)

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLIBSSH_ROOT=$LIBSSH_ROOT \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_partial\;imas_remote $*
