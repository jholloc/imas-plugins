find_package( UDA QUIET )
find_package( Boost QUIET COMPONENTS filesystem system )
find_package( IMAS QUIET )
find_package( MDSplus QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS plugin" )
  return()
elseif( NOT IMAS_FOUND )
  message( WARNING "IMAS not found - skipping IMAS plugin" )
  return()
elseif( NOT MDSplus_FOUND )
  message( WARNING "MDSplus not found - skipping IMAS plugin" )
  return()
endif()

include( plugins )

set( SOURCES imas_plugin.cpp )
set( HEADERS imas_plugin.h )

uda_plugin(
  NAME IMAS
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imasPlugin
  DESCRIPTION "Plugin which handles all requests coming from IMAS AL5 UDA backend"
  EXAMPLE "IMAS::open(uri=imas:hdf5&path=/path/to/file, mode=open)"
  LIBNAME imas_plugin
  SOURCES ${SOURCES}
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${IMAS_LOW_LEVEL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${MDSplus_INCLUDES}
  EXTRA_LINK_DIRS
    ${IMAS_LOW_LEVEL_LIBRARY_DIRS}
    ${MDSplus_LIBRARY_DIRS}
    ${UDA_CLIENT_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${IMAS_LOW_LEVEL_LIBRARIES}
    ${Boost_LIBRARIES}
)

list( TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
list( TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )

set_property( TARGET clang-format APPEND PROPERTY SOURCES ${SOURCES} ${HEADERS} )
