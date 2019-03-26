########################################################################################################################
# Plugins to build

if( NOT DEFINED BUILD_PLUGINS )
  set( BUILD_PLUGINS
    exp2imas
    imas_remote
    imas_local
    imas_old
    imas_mapping
    imasdd
    jet_magnetics
    mast_imas
    source
    west
    imas_uda
    imas_partial
  )
endif()

########################################################################################################################
# Build the plugins

find_package( UDA REQUIRED )

get_filename_component( UDA_HOME ${UDA_LIBRARY_DIRS} DIRECTORY )

include_directories( ${UDA_CPP_INCLUDE_DIRS} )
include_directories( ${CMAKE_CURRENT_LIST_DIR} )

add_library( test_helpers test_helpers.cpp test_helpers.h )

# This list is appended to by the uda_plugin macro
set( BUILT_PLUGINS "" )

foreach( PLUGIN IN ITEMS ${BUILD_PLUGINS} )
  add_subdirectory( ${PLUGIN} )
endforeach( PLUGIN )

########################################################################################################################
# Generate scripts

string( REGEX REPLACE ";" " " PLUGINS "${BUILT_PLUGINS}" )

configure_file(
  ${CMAKE_SOURCE_DIR}/scripts/activate-plugins.sh.in
  ${CMAKE_BINARY_DIR}/scripts/activate-plugins.sh
  @ONLY
)

configure_file(
  ${CMAKE_SOURCE_DIR}/scripts/remove-plugins.sh.in
  ${CMAKE_BINARY_DIR}/scripts/remove-plugins.sh
  @ONLY
)
