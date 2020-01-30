#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module load cineca
module load itmenv
#module load uda/develop
module load uda/2.2.5
module unload imas
module use /gw/swimas/extra/uda/imas/etc/modulefiles

#module load imas/3.21.1-develop-fat-uda
#module load imas/3.21.1-feature/get-partial
#module load imas/3.20.0-3.8.5-fat-uda
module load IMAS/3.23.1/AL/4.1.0

export CC=gcc
export CXX=g++
export BOOST_ROOT=/afs/eufus.eu/user/g/g2jhollo/boost_1_62_0

#    -DBUILD_PLUGINS=imas_old\;exp2imas
#    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_old
#    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLIBSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
    -DCMAKE_INSTALL_PREFIX=/gw/swimas/extra/uda/plugins/develop \
    -DBUILD_PLUGINS=exp2imas\;imas_remote\;imas_uda\;west_tunnel\;imas_forward
