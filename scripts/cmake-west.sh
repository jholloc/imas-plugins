#!/bin/bash

source /usr/share/Modules/init/bash
module purge
module use /Applications/Modules/soft
module use /Applications/Modules/compilers
module load gcc/6.4.0
module load mdsplus/7.7-4 
module load cmake/3.7.2 

#cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=OTHER -DCMAKE_INSTALL_PREFIX=. \
#    -DLIBTS_ROOT=/Home/devarc/PortageMatlab7/tslib_client2013 \
#    -DBUILD_PLUGINS=imas\;help\;west\;tore_supra $* 


export CC=gcc
export CXX=g++

#cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
#    -DLIBSSH_ROOT=/afs/eufus.eu/user/g/g2jhollo \
#    -DCMAKE_INSTALL_PREFIX=$HOME/uda

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=.


