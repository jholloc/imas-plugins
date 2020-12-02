find_package( UDA QUIET )
find_package( Boost QUIET COMPONENTS filesystem )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS_FORWARD plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS_FORWARD plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMAS_FORWARD
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imasForwardPlugin
  DESCRIPTION ""
  EXAMPLE ""
  LIBNAME imas_forward_plugin
  SOURCES
  imas_forward_plugin.cpp
  imas_forward_plugin.h
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_DIRS
    ${MDSplus_LIBRARY_DIRS}
    ${UDA_CLIENT_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    uda_cpp
)
