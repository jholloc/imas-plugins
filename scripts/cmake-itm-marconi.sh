#!/bin/bash

module ()
{
    eval `/usr/bin/modulecmd bash $*`
}

module purge
module load cineca
module load imasenv
module unload uda
module load uda/2.3.1/gcc/7.3.0

export CC=gcc
export CXX=g++
export BOOST_ROOT=/afs/eufus.eu/user/g/g2jhollo/boost_1_62_0

AL_VERION=$(echo $IMAS_PREFIX | rev | cut -d '/' -f 1 | rev)

#    -DBUILD_PLUGINS=imas_old\;exp2imas
#    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_old
#    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
#    -DBUILD_PLUGINS=exp2imas\;imas_remote\;imas_uda\;west_tunnel\;imas_forward
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DLIBSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
    -DCMAKE_INSTALL_PREFIX=/gw/swimas/extra/uda/plugins/1.1.0/AL/$AL_VERSION \
    -DBUILD_PLUGINS=exp2imas\;imas_mapping\;imasdd\;imas_uda\;west_tunnel\;imas_partial\;imas_remote
