#!/bin/bash

export PKG_CONFIG_PATH=$HOME/Projects/uda/lib/pkgconfig

cmake -GNinja -H. -Bbuild \
    -DCMAKE_INSTALL_PREFIX=$HOME/Projects/uda -DCMAKE_BUILD_TYPE=Debug $*
