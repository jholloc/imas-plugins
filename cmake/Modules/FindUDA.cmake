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

set( UDA_DEFINITIONS  )
set( UDA_INCLUDE_DIRS )
set( UDA_LIBRARY_DIRS )
set( UDA_LIBRARIES )

if( "${UDA_FIND_COMPONENTS}" STREQUAL "" )
  set( UDA_FIND_COMPONENTS client )
endif()

set( UDA_COMPONENTS )
foreach( COMP ${UDA_FIND_COMPONENTS} )
  set( UDA_COMPONENTS ${UDA_COMPONENTS} uda-${COMP} )
endforeach()

pkg_check_modules( PC_UDA QUIET "${UDA_COMPONENTS}" )

set( UDA_DEFINITIONS  ${UDA_DEFINITIONS}  ${PC_UDA_CFLAGS_OTHER} )
set( UDA_INCLUDE_DIRS ${UDA_INCLUDE_DIRS} ${PC_UDA_INCLUDE_DIRS} )
set( UDA_LIBRARY_DIRS ${UDA_LIBRARY_DIRS} ${PC_UDA_LIBRARY_DIRS} )
set( UDA_LIBRARIES    ${UDA_LIBRARIES}    ${PC_UDA_LIBRARIES} )

# handle the QUIETLY and REQUIRED arguments and set UDA_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( UDA DEFAULT_MSG UDA_LIBRARIES UDA_INCLUDE_DIRS )

mark_as_advanced( UDA_INCLUDE_DIRS UDA_LIBRARIES )
