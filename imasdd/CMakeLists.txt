include( plugins )

uda_plugin(
  NAME IMASDD
  VERSION 1.0.0
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
  EXTRA_LINK_LIBS
)