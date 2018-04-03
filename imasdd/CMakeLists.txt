set( SOURCES imasdd_plugin.cpp )
set( HEADERS imasdd_plugin.h pugixml.hpp )
set( VERSION 1.0.0 )

#include( ExternalProject )
#
#ExternalProject_Add( pugixml_ext
#  GIT_REPOSITORY https://github.com/zeux/pugixml
#  GIT_TAG v1.8
#  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/pugixml
#  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/extlibs
#)
#
#include_directories( ${CMAKE_BINARY_DIR}/extlibs/include )
#link_directories( ${CMAKE_BINARY_DIR}/extlibs/lib64 )

add_definitions(
  -DPLUGIN_VERSION=1
  -DPUGIXML_HEADER_ONLY
)
add_library( imasdd_plugin SHARED ${SOURCES} ${HEADERS} )

add_subdirectory( tests )

#add_dependencies( imasdd_plugin pugixml_ext )
#target_link_libraries( imasdd_plugin pugixml )

set_target_properties( imasdd_plugin
  PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${VERSION}
)
