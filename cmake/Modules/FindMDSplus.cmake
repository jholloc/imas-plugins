# - Find MDSplus
# Find the MDSplus includes and libraries
#
# To provide the module with a hint about where to find your MDSplus
# installation, you can set the environment variables MDSplus_DIR. The
# Find module will then look in this path when searching for MDSplus
# paths and libraries.
#
#  MDSplus_INCLUDES    - where to find mdslib.h, etc
#  MDSplus_LIBRARIES   - Link these libraries when using MDSplus
#  MDSplus_FOUND       - True if MDSplus found
#
# Normal usage would be:
#  find_package (MDSplus REQUIRED)
#  target_link_libraries (uses_mdsplus ${MDSplus_LIBRARIES})

if( MDSplus_INCLUDES AND MDSplus_LIBRARIES )
  # Already in cache, be silent
  set( MDSplus_FIND_QUIETLY TRUE )
endif( MDSplus_INCLUDES AND MDSplus_LIBRARIES )

find_path( MDSplus_INCLUDES mdslib.h
  HINTS
    ${MDSplus_DIR}
    ENV MDSplus_DIR
  PATHS
    /usr/local/mdsplus
  PATH_SUFFIXES include )

set( MDS_LIBS
  MdsObjectsCppShr MdsLib
)

set( MDSplus_LIBRARIES "" )

foreach( MDS_LIB ${MDS_LIBS} )

  find_library( ${MDS_LIB}-FIND NAMES "${MDS_LIB}"
    HINTS
      ${MDSplus_DIR}
      ENV MDSplus_DIR
    PATHS
      /usr/local/mdsplus
    PATH_SUFFIXES lib lib64 )

  if( ${MDS_LIB}-FIND )
    list( APPEND MDSplus_LIBRARIES "${${MDS_LIB}-FIND}" )
    get_filename_component( MDSplus_LIBRARY_DIRS "${${MDS_LIB}-FIND}" DIRECTORY )
  else()
    if( MDSplus_FIND_REQUIRED )
      message( FATAL_ERROR "Failed to find MDS library: ${MDS_LIB}" )
    endif()
  endif()

endforeach()

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MDSplus DEFAULT_MSG MDSplus_LIBRARIES MDSplus_INCLUDES )

mark_as_advanced( MDSplus_LIBRARIES MDSplus_INCLUDES )
