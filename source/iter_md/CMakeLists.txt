find_package( UDA QUIET )
find_package( Boost QUIET )
find_package( ITERMD QUIET )
find_package( PostgreSQL QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping ITER_MD plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping ITER_MD plugin" )
  return()
elseif( NOT ITERMD_FOUND )
  message( WARNING "ITERMD not found - skipping ITER_MD plugin" )
  return()
elseif( NOT PostgreSQL_FOUND )
  message( WARNING "PostgreSQL not found - skipping ITER_MD plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME ITER_MD
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC itermdPlugin
  DESCRIPTION ""
  EXAMPLE ""
  LIBNAME iter_md_plugin
  SOURCES
    iter_md_plugin.cpp
    iter_md_plugin.h
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${ITERMD_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIR}
  EXTRA_LINK_DIRS
    /opt/local/lib
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITERMD_LIBRARIES}
    ${PostgreSQL_LIBRARY}
    uda_cpp
)
