find_package( UDA QUIET )
find_package( Boost QUIET )

if( NOT UDA_FOUND )
  message( WARNING "UDA not found - skipping imas_mapping plugin" )
  return()
elseif( NOT Boost_FOUND )
  message( WARNING "Boost not found - skipping imas_mapping plugin" )
  return()
endif()

include( plugins )

uda_plugin(
  NAME IMAS_MAPPING
  VERSION 1.0.0
  ENTRY_FUNC imas_mapping_plugin
  DESCRIPTION "Plugin to handle data requests from (new) IMAS low level and select the appropriate plugin to map the request to experimental data"
  EXAMPLE "IMAS_MAPPING::get(experiment=JET, shot=84600, run=0, ids=magnetics, path=flux_loop/3/flux/data, rank=1, type=double)"
  LIBNAME imas_mapping_plugin
  SOURCES
    imas_mapping_plugin.cpp
    imas_mapping_plugin.h
    machine_mapping.cpp
    machine_mapping.h
  EXTRA_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${UDA_CLIENT_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
)

add_subdirectory( tests )
