find_package( UDA REQUIRED )
find_package( Boost REQUIRED filesystem system )


include( plugins )

set( SOURCES
  jet_equilibrium_plugin.cpp
)

include_directories( SYSTEM ${Boost_INCLUDE_DIR} )

uda_plugin(
  NAME JET_EQUILIBRIUM
  ENTRY_FUNC jetEquilibriumPlugin
  DESCRIPTION "Map IMAS element name to experiment static or dynamic data using XML mapping files"
  EXAMPLE "JET_EQUILIBRIUM::read()"
  LIBNAME jet_equilibrium_plugin
  SOURCES ${SOURCES}
  #CONFIG_FILE exp2imas.cfg
  VERSION ${PROJECT_VERSION}
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_DIRS
    ${UDA_CLIENT_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${Boost_LIBRARIES}
)
