#!/bin/bash

module purge
module load cineca
module load mdsplus/7.46-1 
module load cmake/3.5.2

export PKG_CONFIG_PATH=$HOME/public/uda/lib/pkgconfig

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/public/uda \
    -DBUILD_PLUGINS=imas_mapping\;hl2a\;tore_supra \
