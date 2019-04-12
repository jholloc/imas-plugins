#!/bin/bash

export PKG_CONFIG_PATH=$HOME/Projects/uda-develop/lib/pkgconfig:$HOME/Projects/IMAS/installer/src/3.21.1/ual/develop/lowlevel

cmake -GNinja -H. -Bbuild \
    -DCMAKE_INSTALL_PREFIX=$HOME/Projects/uda-develop -DCMAKE_BUILD_TYPE=Debug $*
