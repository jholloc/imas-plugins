find_package( UDA REQUIRED )
find_package( Boost REQUIRED filesystem system )
find_package( MDSplus QUIET )

if( NOT MDSPLUS_FOUND )
  message( WARNING "mdsplus not found - skipping jet_summary plugin" )
  return()
endif()

include( plugins )

set( SOURCES
  jet_summary_plugin.cpp
)

include_directories( SYSTEM ${Boost_INCLUDE_DIR} )

uda_plugin(
  NAME JET_SUMMARY
  ENTRY_FUNC jetSummaryPlugin
  DESCRIPTION "Map IMAS element name to experiment static or dynamic data using XML mapping files"
  EXAMPLE "JET_SUMMARY::read()"
  LIBNAME jet_summary_plugin
  SOURCES ${SOURCES}
  #CONFIG_FILE exp2imas.cfg
  VERSION ${PROJECT_VERSION}
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${MDSPLUS_INCLUDES}
  EXTRA_LINK_DIRS
    ${UDA_CLIENT_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${MDSPLUS_LIBRARIES}
    ${Boost_LIBRARIES}
)
