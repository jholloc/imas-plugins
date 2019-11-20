find_package( LibXml2 QUIET )
find_package( MDSplus QUIET )
find_package( HDF5 COMPONENTS C HL QUIET )
find_package( OpenSSL QUIET )

if( NOT MDSPLUS_FOUND )
  message( WARNING "mdsplus not found - skipping IMAS plugin" )
  return()
elseif( NOT LIBXML2_FOUND )
  message( WARNING "Libxml2 not found - skipping IMAS plugin" )
  return()
elseif( NOT HDF5_FOUND )
  message( WARNING "HDF5 not found - skipping IMAS plugin" )
  return()
elseif( NOT OPENSSL_FOUND )
  message( WARNING "OpenSSL not found - skipping IMAS plugin" )
  return()
endif()

add_definitions( -Wno-error -Wno-all )

include( plugins )

set( IMAS_SOURCES 
  ual_low_level_mdsplus.c
  ual_low_level_remote.c
  ual_low_level.c
  ual_low_level_meta.cpp
  ual_low_level_mdsobjects.cpp
  ual_low_level_idam_dummy.c
)

uda_plugin(
  NAME IMAS
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imas_mds
  DESCRIPTION "ITER IMAS put & get plugin"
  EXAMPLE "IMAS::put()"
  LIBNAME imas_plugin
  SOURCES
    common.c
    extract_indices.c
    imas_common.c
    imas_hdf5.c
    imas_mds.c
    machine_mapping.cpp
    ${IMAS_SOURCES}
  CONFIG_FILE imas.cfg
  EXTRA_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${MDSPLUS_INCLUDES}
    ${CMAKE_SOURCE_DIR}/client
    ${HDF5_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${LIBXML2_LIBRARIES}
    ${MDSPLUS_LIBRARIES}
    TreeShr
    MdsLib
    MdsShr
    MdsIpShr
    TdiShr
    ${HDF5_LIBRARIES}
    ${HDF5_HL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    uda_client
)
