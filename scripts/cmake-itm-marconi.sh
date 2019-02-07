#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module load cineca
module load itmenv
module load uda/2.2.3
module unload imas
module use /gw/swimas/extra/uda/imas/etc/modulefiles

module load imas/3.21.1-develop-uda

export CC=gcc
export CXX=g++
export BOOST_ROOT=/afs/eufus.eu/user/g/g2jhollo/boost_1_62_0

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLIBSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
    -DCMAKE_INSTALL_PREFIX=/gw/swimas/extra/uda/2.2.3 \
    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_partial
