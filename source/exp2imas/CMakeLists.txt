find_package( LibXml2 QUIET )
find_package( MDSplus QUIET )
find_package( LibSSH QUIET )
find_package( GTK3 QUIET )
find_package( UDA REQUIRED )
find_package( Boost REQUIRED filesystem system )

if( NOT MDSplus_FOUND )
  message( WARNING "mdsplus not found - skipping exp2imas plugin" )
  return()
elseif( NOT LIBXML2_FOUND )
  message( WARNING "Libxml2 not found - skipping exp2imas plugin" )
  return()
elseif( NOT LibSSH_FOUND )
  message( WARNING "Libssh not found - skipping exp2imas plugin" )
  return()
#elseif( NOT GTK3_FOUND )
  # Currently does not work without GTK3 gui
  #message( WARNING "GTK3 not found - skipping exp2imas plugin" )
  #return()
endif()

include( plugins )

set( SOURCES
  exp2imas_plugin.cpp
  exp2imas_mds.cpp
  exp2imas_xml.cpp
  exp2imas_ssh_server.c
  exp2imas_ssh.c
  exp2imas_ramCache.c
  exp2imas_mapping_files.cpp
)

if( GTK3_FOUND )
  link_directories( ${GTK3_LIBRARY_DIRS} )
  add_definitions( -DHAVE_GTK3 )

  set( SOURCES exp2imas_ssh_gui.c ${SOURCES} )
endif()

include_directories( SYSTEM ${Boost_INCLUDE_DIR} )

uda_plugin(
  NAME EXP2IMAS
  ENTRY_FUNC exp2imasPlugin
  DESCRIPTION "Map IMAS element name to experiment static or dynamic data using XML mapping files"
  EXAMPLE "EXP2IMAS::read()"
  LIBNAME exp2imas_plugin
  SOURCES ${SOURCES}
  CONFIG_FILE exp2imas.cfg
  VERSION ${PROJECT_VERSION}
  EXTRA_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${MDSplus_INCLUDES}
    ${LibSSH_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_DIRS
    ${UDA_CLIENT_LIBRARY_DIRS}
  EXTRA_LINK_LIBS
    ${LIBXML2_LIBRARIES}
    ${MDSplus_LIBRARIES}
    ${LibSSHLIBRARIES}
    ${GTK3_LIBRARIES}
    ${UDA_CLIENT_LIBRARIES}
    ${Boost_LIBRARIES}
    pthread
    resolv
  COPY_ASSET_DIR mappings
)

add_subdirectory( tests )
