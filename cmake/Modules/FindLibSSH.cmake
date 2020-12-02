# - Try to find LibSSH
# Once done this will define
#
#  LibSSH_FOUND - system has LibSSH
#  LibSSH_INCLUDE_DIRS - the LibSSH include directory
#  LibSSH_LIBRARIES - Link these to use LibSSH
#  LibSSH_DEFINITIONS - Compiler switches required for using LibSSH
#

if( LibSSH_LIBRARIES AND LibSSH_INCLUDE_DIRS )
  # Already in cache, be silent
  set( LibSSH_FIND_QUIETLY TRUE )
endif( LibSSH_LIBRARIES AND LibSSH_INCLUDE_DIRS )

find_path( LibSSH_INCLUDE_DIRS libssh/libssh.h
  HINTS ${LibSSH_ROOT}
    ENV LibSSH_DIR
  PATHS
    /usr/local
    /opt/local
    /sw
    /usr/lib/sfw
  PATH_SUFFIXES include )

find_library( LibSSH_LIBRARIES NAMES ssh
  HINTS ${LibSSH_ROOT}
    ENV LibSSH_DIR
  PATHS
    /opt/local
    /sw
    /usr
    /usr/local
  PATH_SUFFIXES lib lib64 )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( LibSSH DEFAULT_MSG LibSSH_LIBRARIES LibSSH_INCLUDE_DIRS )

# show the LibSSH_INCLUDE_DIRS and LibSSH_LIBRARIES variables only in the advanced view
mark_as_advanced( LibSSH_INCLUDE_DIRS LibSSH_LIBRARIES )
