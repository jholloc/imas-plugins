find_package( Boost REQUIRED )

include( plugins )

set( SOURCES
  jet_magnetics_plugin.cpp
  tinyxml2.cpp
)

uda_plugin(
  NAME JET_MAGNETICS
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC jetMagneticsPlugin
  DESCRIPTION ""
  EXAMPLE "JET_MAGNETICS::read()"
  LIBNAME jet_magnetics_plugin
  SOURCES ${SOURCES}
  CONFIG_FILE jet_magnetics.cfg
  VERSION ${PROJECT_VERSION}
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${Boost_LIBRARIES}
)
