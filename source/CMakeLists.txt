########################################################################################################################
# Plugins to build

if( NOT DEFINED BUILD_PLUGINS )
  set( BUILD_PLUGINS
    exp2imas
    hl2a
    imas_forward
    imas_mapping
    imas_old
    imas_partial
    imas_remote
    imas_uda
    imasdd
    iter_md
    #livedisplay
    jet_equilibrium
    jet_magnetics
    jet_summary
    mast_imas
    source
    tcv
    tcvm
    tore_supra
    west
    west_tunnel
  )
endif()

########################################################################################################################
# Build the plugins

find_package( UDA REQUIRED )

list( GET UDA_DIR 0 UDA_LIBRARY_DIR )
get_filename_component( UDA_HOME ${UDA_LIBRARY_DIR} DIRECTORY )

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
