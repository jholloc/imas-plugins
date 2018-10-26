#!/bin/bash

source /usr/share/Modules/init/bash
module purge
module use /Applications/Modules/soft
module use /Applications/Modules/compilers
module load gcc/6.4.0
module load mdsplus/7.7-4 
module load cmake/3.7.2
module load blitz
#module use ~/imas_modules/etc/modulefiles
module use ~/privatemodules
module use /ZONE_TRAVAIL/LF218007/imas/etc/modulefiles
#module load imas/improvement/new_low_level_3.20.0/ual/release/4.0.0
#module show imas/improvement/new_low_level_3.20.0/ual/release/4.0.0
#module load imas/improvement/new_low_level_rebased_3.18.0_structure_gen/ual/release/4.0.0
module unload uda/newLL
module load uda/uda_for_new_ll
module load hdf5
export BOOST_ROOT=/Applications/boost_1_66_0
#export PKG_CONFIG_PATH=~/imas_modules/core/imas/improvement/new_low_level_3.20.0/ual/release/4.0.0/lib/pkgconfig/:$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=/ZONE_TRAVAIL/LF218007/imas/core/imas/move_to_newLL_3.21.1/ual/improvement/testing4.0.0/lib/pkgconfig/:$PKG_CONFIG_PATH

cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/Home/LF218007/uda_test_newll/uda \
    -DBUILD_PLUGINS=imas_mapping\;west \
    -DLIBTS_ROOT=/Home/devarc/PortageMatlab7/tslib_client2013 \
