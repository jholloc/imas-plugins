find_package( UDA REQUIRED )

include_directories( ${UDA_CPP_INCLUDE_DIRS} )
include_directories( ${CMAKE_CURRENT_LIST_DIR} )

add_library( test_helpers test_helpers.cpp test_helpers.h )

########################################################################################################################
# Plugins to build

if( DEFINED BUILD_PLUGINS )
  foreach( PLUGIN IN ITEMS ${BUILD_PLUGINS} )
    add_subdirectory( ${PLUGIN} )
  endforeach( PLUGIN )
else()
  add_subdirectory( exp2imas )
  add_subdirectory( imas )
  add_subdirectory( imas_mapping )
  add_subdirectory( imasdd )
  add_subdirectory( mast_imas )
  add_subdirectory( source )
  add_subdirectory( tore_supra )
  add_subdirectory( west )
endif()