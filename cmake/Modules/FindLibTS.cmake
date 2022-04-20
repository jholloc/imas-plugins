# - Try to find LibTS
# Once done this will define
#
#  LibTS_FOUND - system has LibTS
#  LibTS_INCLUDE_DIRS - the LibTS include directory
#  LibTS_LIBRARIES - Link these to use LibTS
#  LibTS_DEFINITIONS - Compiler switches required for using LibTS
#

if( LibTS_LIBRARIES AND LibTS_INCLUDE_DIRS )
  # Already in cache, be silent
  set( LibTS_FIND_QUIETLY TRUE )
endif( LibTS_LIBRARIES AND LibTS_INCLUDE_DIRS )

find_path( LibTS_INCLUDE_DIRS tsdef.h
  HINTS ${LibTS_ROOT}
    ENV LIBTS_DIR
  PATHS
    /usr/local
    /opt/local
    /sw
    /usr/lib/sfw
  PATH_SUFFIXES include )

find_library( LibTS_LIBRARIES NAMES TS
  HINTS ${LibTS_ROOT}
    ENV LIBTS_DIR
  PATHS
    /opt/local
    /sw
    /usr
    /usr/local
  PATH_SUFFIXES lib lib64 )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( LibTS DEFAULT_MSG LibTS_LIBRARIES LibTS_INCLUDE_DIRS )

# show the LibTS_INCLUDE_DIRS and LibTS_LIBRARIES variables only in the advanced view
mark_as_advanced( LibTS_INCLUDE_DIRS LibTS_LIBRARIES )
