#!/bin/bash

module purge
module load cineca
module load cmake/3.5.2
module load itm-gcc/6.1.0 
module load mdsplus/7.46-1 

export UDA_HOME=$HOME/public/uda
export PKG_CONFIG_PATH=$UDA_HOME/lib/pkgconfig

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
    -DBUILD_PLUGINS=imas_mapping\;hl2a\;tore_supra \
