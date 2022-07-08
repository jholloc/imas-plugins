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

uda_plugin(
  NAME IMAS
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imasPlugin
  DESCRIPTION ""
  EXAMPLE ""
  LIBNAME imas_plugin
  SOURCES
    imas_plugin.cpp
    imas_plugin.h
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
