find_package( UDA QUIET )
find_package( IMAS QUIET )
find_package( Boost QUIET )
find_package( MDSplus QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS_PARTIAL plugin" )
  return()
elseif( NOT IMAS_FOUND )
  message( WARNING "IMAS not found - skipping IMAS_PARTIAL plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS_PARTIAL plugin" )
  return()
elseif( NOT MDSPLUS_FOUND )
  message( WARNING "MDSplus not found - skipping IMAS_PARTIAL plugin" )
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
    ${Boost_INCLUDE_DIRS}
    ${MDSPLUS_INCLUDES}
  EXTRA_LINK_DIRS
    ${IMAS_LOW_LEVEL_LIBRARY_DIRS}
    ${MDSPLUS_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${IMAS_LOW_LEVEL_LIBRARIES}
    MdsObjectsCppShr
    uda_cpp
)

enable_testing()
add_subdirectory( tests )
