find_package( UDA QUIET )
find_package( Boost QUIET COMPONENTS filesystem system )
find_package( IMAS QUIET )
find_package( CURL QUIET )

option( ENABLE_AUTHORISATION "Enable authorisation in IMAS plugin" OFF )

set( IMAS_DEFINITIONS "" )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS plugin" )
  return()
elseif( NOT CURL_FOUND )
  message( WARNING "CURL not found - skipping IMAS plugin" )
  return()
elseif( NOT IMAS_FOUND )
  message( WARNING "IMAS not found - building for MAPPING only" )
  list( APPEND IMAS_DEFINITIONS "-DNO_IMAS" )
endif()

if( ENABLE_AUTHORISATION )
  list( APPEND IMAS_DEFINITIONS "-DUDA_AUTHORISATION" )
endif(  )

include( plugins )

set( SOURCES imas_plugin.cpp curl_wrapper.cpp )
set( HEADERS imas_plugin.h )

uda_plugin(
  NAME IMAS
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imasPlugin
  DESCRIPTION "Plugin which handles all requests coming from IMAS AL5 UDA backend"
  EXAMPLE "IMAS::open(uri=imas:hdf5&path=/path/to/file, mode=open)"
  LIBNAME imas_plugin
  CONFIG_FILE imas.cfg
  SOURCES ${SOURCES}
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${IMAS_LOW_LEVEL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
  EXTRA_LINK_DIRS
    ${IMAS_LOW_LEVEL_LIBRARY_DIRS}
    ${UDA_CLIENT_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${IMAS_LOW_LEVEL_LIBRARIES}
    ${Boost_LIBRARIES}
    CURL::libcurl
  EXTRA_DEFINITIONS
    ${IMAS_DEFINITIONS}
)

list( TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
list( TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )

set_property( TARGET clang-format APPEND PROPERTY SOURCES ${SOURCES} ${HEADERS} )
