#!/bin/bash

source /usr/share/Modules/init/bash
module purge
module use /Applications/Modules/soft
module use /Applications/Modules/compilers
module load gcc/6.4.0
module load mdsplus/7.46-4 
module load cmake/3.7.2
module load blitz
#module load hdf5

export BOOST_ROOT=/Applications/boost_1_69_0
export PKG_CONFIG_PATH=/home/LF218007/public/UDA_TUTORIAL/uda/lib/pkgconfig

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/home/LF218007/public/UDA_TUTORIAL/uda \
    -DBUILD_PLUGINS=imas_mapping\;hl2a \
#    -DLIBTS_ROOT=/Home/devarc/PortageMatlab7/tslib_client2013 \
