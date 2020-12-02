#!/bin/bash

module ()
{
    eval $(/usr/bin/modulecmd bash "$@")
}

module purge
module load cineca
module load itmenv
module load uda/2.2.5
module unload imas

module load IMAS/3.24.0/AL/4.1.5

export CC=gcc
export CXX=g++
export BOOST_ROOT=/afs/eufus.eu/user/g/g2jhollo/boost_1_62_0

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLibSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
    -DCMAKE_INSTALL_PREFIX="$UDA_HOME" \
    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_partial\;imas_remote
