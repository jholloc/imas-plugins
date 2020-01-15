#!/bin/bash

source /usr/share/Modules/init/bash
module purge
module use /Applications/Modules/soft
module use /Applications/Modules/compilers
module load gcc/6.4.0
module load mdsplus/7.92-0 
module load cmake/3.7.2
module load blitz

export BOOST_ROOT=/Applications/boost_1_66_0
export PKG_CONFIG_PATH=$UDA_HOME/lib/pkgconfig

cmake -Bbuild -H. \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
    -DBUILD_PLUGINS=imas_mapping\;west\
    -DLIBTS_ROOT=/Home/devarc/PortageMatlab7/tslib_client2013 \
