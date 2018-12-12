# Set up environment for compilation
. /usr/share/Modules/init/sh
module use /work/imas/etc/modulefiles

module purge

module load cmake/3.0.2
module load Boost/1.66.0-foss-2018a
module load openssl/1.0.2g
module load MDSplus/7.7.12-intel-2018a

make -C build $*
