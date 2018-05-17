cmake_minimum_required( VERSION 3.0 )

########################################################################################################################
# CMake configuration
#
cmake_policy( SET CMP0042 NEW )
cmake_policy( SET CMP0048 NEW )
if( "${CMAKE_VERSION}" VERSION_GREATER "3.8.0" )
  cmake_policy( SET CMP0068 NEW )
endif()

# Extra cmake modules to find libraries
set( CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)

project( imas_plugins )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

include( LibXml2StrPrintfConfig )

########################################################################################################################
# Plugins to build

if( DEFINED BUILD_PLUGINS )
  foreach( PLUGIN IN ITEMS ${BUILD_PLUGINS} )
    add_subdirectory( ${PLUGIN} )
  endforeach( PLUGIN )
else()
  add_subdirectory( exp2imas )
  #add_subdirectory( imas )
  add_subdirectory( imasdd )
  add_subdirectory( imas_mapping )
  #add_subdirectory( tore_supra )
  #add_subdirectory( west )
endif()
