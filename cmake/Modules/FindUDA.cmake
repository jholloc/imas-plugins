# - Find UDA
#
# Once done this will define
#
#  UDA_FOUND - System has UDA
#  UDA_INCLUDE_DIRS - The UDA include directory
#  UDA_LIBRARIES - The libraries needed to use UDA
#  UDA_DEFINITIONS - Compiler switches required for using UDA
#=======================================================================================================================

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
find_package( PkgConfig )

if( "${UDA_FIND_COMPONENTS}" STREQUAL "" )
  set( UDA_FIND_COMPONENTS client )
endif()

set( UDA_COMPONENTS )
set( UDA_FAT_COMPONENTS ${UDA_COMPONENTS} uda-fat-client uda-fat-cpp )

foreach( COMP ${UDA_FIND_COMPONENTS} )
  set( UDA_COMPONENTS ${UDA_COMPONENTS} uda-${COMP} )
  if( "${COMP}" STREQUAL "client" OR "${COMP}" STREQUAL "cpp" )
    set( UDA_FAT_COMPONENTS ${UDA_FAT_COMPONENTS} uda-fat-${COMP} )
  endif()
endforeach()

pkg_check_modules( PC_UDA QUIET "${UDA_COMPONENTS}" )
if( NOT "${UDA_FAT_COMPONENTS}" STREQUAL "" )
  pkg_check_modules( PC_FAT_UDA QUIET "${UDA_FAT_COMPONENTS}" )
endif()

set( UDA_DEFINITIONS  ${PC_UDA_CFLAGS_OTHER} )
set( UDA_INCLUDE_DIRS ${PC_UDA_INCLUDE_DIRS} )
set( UDA_LIBRARY_DIRS ${PC_UDA_LIBRARY_DIRS} )
set( UDA_LIBRARIES    ${PC_UDA_LIBRARIES} )

set( FAT_UDA_DEFINITIONS  ${PC_FAT_UDA_CFLAGS_OTHER} )
set( FAT_UDA_INCLUDE_DIRS ${PC_FAT_UDA_INCLUDE_DIRS} )
set( FAT_UDA_LIBRARY_DIRS ${PC_FAT_UDA_LIBRARY_DIRS} )
set( FAT_UDA_LIBRARIES    ${PC_FAT_UDA_LIBRARIES} )

# handle the QUIETLY and REQUIRED arguments and set UDA_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( UDA DEFAULT_MSG UDA_LIBRARIES UDA_INCLUDE_DIRS )
find_package_handle_standard_args( FAT_UDA DEFAULT_MSG FAT_UDA_LIBRARIES FAT_UDA_INCLUDE_DIRS )

mark_as_advanced( UDA_INCLUDE_DIRS UDA_LIBRARIES )
mark_as_advanced( FAT_UDA_INCLUDE_DIRS FAT_UDA_LIBRARIES )
