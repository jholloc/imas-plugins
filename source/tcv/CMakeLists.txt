find_package( LibXml2 QUIET )
find_package( MDSplus QUIET )

if( NOT MDSPLUS_FOUND )
  message( WARNING "mdsplus not found - skipping tcv plugin" )
  return()
elseif( NOT LIBXML2_FOUND )
  message( WARNING "Libxml2 not found - skipping tcv plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME TCV
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC tcvPlugin
  DESCRIPTION "Read TCV data"
  EXAMPLE "TCV::read()"
  LIBNAME tcv_plugin
  SOURCES tcv_plugin.c tcv_mds.c tcv_xml.c
  CONFIG_FILE tcv.cfg
  EXTRA_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${MDSPLUS_INCLUDES}
  EXTRA_LINK_LIBS
    ${LIBXML2_LIBRARIES}
    ${MDSPLUS_LIBRARIES}
)
