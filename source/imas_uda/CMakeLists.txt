find_package( UDA QUIET )
find_package( Boost QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping IMAS_UDA plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping IMAS_UDA plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMAS_UDA
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imasUdaPlugin
  DESCRIPTION "Standardised Plugin Template"
  EXAMPLE "IMAS_UDA::functionName()"
  LIBNAME imas_uda_plugin
  SOURCES imasUdaPlugin.cpp
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
)
