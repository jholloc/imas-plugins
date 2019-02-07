find_package( UDA QUIET )
find_package( IMAS QUIET ) 

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping imas_partial plugin" )
  return()
elseif( NOT IMAS_FOUND )
  message( WARNING "IMAS not found - skipping imas_partial plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMAS_PARTIAL
  VERSION 1.0.0
  ENTRY_FUNC imasPartial
  DESCRIPTION "Plugin to expand IMAS get-partial requests and return all required data"
  EXAMPLE "IMAS_PARTIAL::get(path=/magnetics)"
  LIBNAME imas_partial_plugin
  SOURCES
    imas_partial_plugin.cpp
    imas_partial_plugin.h
    access_functions.cpp
    access_functions.h
    pugixml.hpp
    pugiconfig.hpp
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${IMAS_LOW_LEVEL_INCLUDE_DIRS}
  EXTRA_LINK_DIRS
    ${IMAS_LOW_LEVEL_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${IMAS_LOW_LEVEL_LIBRARIES}
)
