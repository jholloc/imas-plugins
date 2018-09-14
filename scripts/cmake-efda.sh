#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module use /afs/eufus.eu/user/g/g2jhollo/uda/modulefiles
module load cineca
module load mdsplus/alpha
module load cmake/3.5.2
module load gnu/6.1.0
module load uda/2.1.0
module load blitz/1.0.1

export PKG_CONFIG_PATH=/afs/eufus.eu/user/g/g2jhollo/IMAS/access-layer/lib/pkgconfig:$PKG_CONFIG_PATH

export CC=gcc
export CXX=g++

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLIBSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
    -DCMAKE_INSTALL_PREFIX=$HOME/uda
