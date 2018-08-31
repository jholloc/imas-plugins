find_package( UDA QUIET )
find_package( Boost QUIET )
find_package( IMAS QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS plugin" )
  return()
elseif( NOT IMAS_FOUND )
  message( WARNING "IMAS not found - skipping IMAS plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMAS
  VERSION 1.0.0
  ENTRY_FUNC imasPlugin
  DESCRIPTION ""
  EXAMPLE ""
  LIBNAME imas_plugin
  SOURCES
    imas_plugin.cpp
    imas_plugin.h
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    /Users/jhollocombe/Projects/access-layer/lowlevel
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    /Users/jhollocombe/Projects/access-layer/lowlevel/
)
