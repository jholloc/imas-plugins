cmake_minimum_required( VERSION 3.1 )

set( CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)

project( uda_plugins )

set( CMAKE_CXX_STANDARD 11 )
#set( CMAKE_POSITION_INDEPENDENT_CODE ON )

find_package( UDA REQUIRED )

include_directories( ${UDA_INCLUDE_DIRS} )

add_definitions( -DSERVERBUILD )

#add_subdirectory( exp2imas )
#add_subdirectory( imas )
add_subdirectory( imasdd )
#add_subdirectory( imas_mapping )
#add_subdirectory( tore_supra )
#add_subdirectory( west )
