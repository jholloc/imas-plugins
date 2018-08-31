find_package( PostgreSQL QUIET )

if( NOT PostgreSQL_FOUND )
  message( WARNING "PostgreSQL not found - skipping mast_imas plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME MAST_IMAS
  ENTRY_FUNC mastImasPlugin
  DESCRIPTION "plugin to handle IMAS requests by returning MAST data"
  EXAMPLE "MAST_IMAS::read(element='magnetics/method/#/name', index=1)"
  LIBNAME mast_imas_plugin
  SOURCES mastImasPlugin.c
  EXTRA_INCLUDE_DIRS
    ${PostgreSQL_INCLUDE_DIR}
  EXTRA_LINK_LIBS
    ${PostgreSQL_LIBRARY}
)
