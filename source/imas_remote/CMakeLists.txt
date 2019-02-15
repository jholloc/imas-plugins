find_package( UDA QUIET )
find_package( Boost QUIET )
find_package( IMAS QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS_REMOTE plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS_REMOTE plugin" )
  return()
elseif( NOT IMAS_FOUND )
  message( WARNING "IMAS not found - skipping IMAS_REMOTE plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMAS_REMOTE
  VERSION 1.0.0
  ENTRY_FUNC imasPlugin
  DESCRIPTION ""
  EXAMPLE ""
  LIBNAME imas_remote_plugin
  SOURCES
    imas_remote_plugin.cpp
    imas_remote_plugin.h
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${IMAS_LOW_LEVEL_INCLUDE_DIRS}
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_DIRS
    ${IMAS_LOW_LEVEL_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${IMAS_LOW_LEVEL_LIBRARIES}
)
