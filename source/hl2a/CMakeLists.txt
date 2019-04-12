find_package( LibXml2 QUIET )
find_package( MDSplus QUIET )

if( NOT MDSPLUS_FOUND )
  message( WARNING "mdsplus not found - skipping hl2a plugin" )
  return()
elseif( NOT LIBXML2_FOUND )
  message( WARNING "Libxml2 not found - skipping hl2a plugin" )
  return()
endif()

add_definitions( -Wno-error )

include( plugins )

uda_plugin(
  NAME HL2A
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC hl2aPlugin
  DESCRIPTION "Read HL2A data"
  EXAMPLE "ADAS::adf21()"
  LIBNAME hl2a_plugin
  SOURCES hl2a_plugin.c hl2a_mapped_data.c hl2a_magnetics.c hl2a_dyn_data_utilities.c hl2a_soft_x_rays.c
  CONFIG_FILE hl2a.cfg
  EXTRA_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${MDSPLUS_INCLUDES}
  EXTRA_LINK_LIBS
    ${LIBXML2_LIBRARIES}
    ${MDSPLUS_LIBRARIES}
)
