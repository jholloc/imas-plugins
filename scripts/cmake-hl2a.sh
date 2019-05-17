#!/bin/bash

source /usr/share/Modules/init/bash
module purge
module use /Applications/Modules/soft
module use /Applications/Modules/compilers
module load gcc/6.4.0
module load mdsplus/7.46-4 
module load cmake/3.7.2
export BOOST_ROOT=/Applications/boost_1_69_0
export PKG_CONFIG_PATH=/home/XL016373/uda/lib/pkgconfig
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/home/XL016373/uda \
    -DBUILD_PLUGINS=imas_mapping\;hl2a \
