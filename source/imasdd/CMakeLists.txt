find_package( UDA QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping imasdd plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMASDD
  VERSION ${PROJECT_VERSION}
  ENTRY_FUNC imasdd_plugin
  DESCRIPTION "Plugin to expand IMAS paths using the data dictionary"
  EXAMPLE "IMASDD::get(path=/magnetics)"
  LIBNAME imasdd_plugin
  SOURCES
    imasdd_plugin.cpp
    imasdd_plugin.h
    pugixml.hpp
    pugiconfig.hpp
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
)

enable_testing()
add_subdirectory( tests )