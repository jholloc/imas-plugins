#!/bin/bash

UDA_HOME="$HOME/Projects/uda"
IMAS_HOME="$HOME/imas"

export PKG_CONFIG_PATH=$UDA_HOME/lib/pkgconfig

cmake -GNinja -H. -Bbuild \
    -DCMAKE_INSTALL_PREFIX="$HOME/Projects/uda" \
    -DCMAKE_BUILD_TYPE=Debug "$@"
