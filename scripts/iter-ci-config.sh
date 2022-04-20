#!/bin/bash
# Bamboo Build script
# Stage 1 : Configure stage

# Set up environment for compilation
. scripts/iter-ci-setup-env.sh || exit 1

UDA_HOME=/home/ITER/hollocj/uda
export ITERMD_ROOT=/home/ITER/hollocj/m-machine-description
export LIBSSH_DIR=$HOME/libssh
export PKG_CONFIG_PATH=$UDA_DIR/lib/pkgconfig:$HOME/IMAS/access-layer/lib/pkgconfig:$PKG_CONFIG_PATH

CC=gcc CXX=g++ cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$UDA_HOME \
    -DBOOST_ROOT="${EBROOTBOOST}" \
    -DPostgreSQL_ROOT="${EBROOTPOSTGRESQL}"
