#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module load cineca
module load mdsplus/alpha
module load cmake/3.5.2
module load gnu/6.1.0
module load blitz/1.0.1
module load uda/2.2.2

export PKG_CONFIG_PATH=/afs/eufus.eu/user/g/g2jhollo/IMAS/access-layer/lib/pkgconfig:$PKG_CONFIG_PATH
export BOOST_ROOT=/afs/eufus.eu/user/g/g2jhollo/boost_1_62_0

export CC=gcc
export CXX=g++

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLIBSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
    -DCMAKE_INSTALL_PREFIX=/gw/swimas/extra/uda/2.2.2 \
    -DBUILD_PLUGINS=exp2imas
