find_package( Boost REQUIRED )

include( plugins )

set( SOURCES
  jet_magnetics_plugin.cpp
  tinyxml2.cpp
)

uda_plugin(
  NAME JET_MAGNETICS
  ENTRY_FUNC jetMagneticsPlugin
  DESCRIPTION ""
  EXAMPLE "JET_MAGNETICS::read()"
  LIBNAME jet_magnetics_plugin
  SOURCES ${SOURCES}
  CONFIG_FILE jet_magnetics.cfg
  VERSION 1.0.0
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${Boost_LIBRARIES}
)
