#!/bin/bash

source /usr/share/Modules/init/bash
module purge
module use /Applications/Modules/soft
module use /Applications/Modules/compilers
module load gcc/6.4.0
module load mdsplus/7.7-4 
module load cmake/3.7.2
module load blitz

export PKG_CONFIG_PATH=/work/imas/core/imas/improvement/new_low_level_3.20.0/ual/release/4.0.0/lib/pkgconfig:/Imas_public/uda_newLL/lib/pkgconfig:$PKG_CONFIG_PATH
export BOOST_ROOT=/Applications/boost_1_66_0
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/Imas_public/uda_newLL \
    -DBUILD_PLUGINS=imas\;west \
    -DLIBTS_ROOT=/Home/devarc/PortageMatlab7/tslib_client2013 \
