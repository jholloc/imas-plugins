include( plugins )

uda_plugin(
  NAME IMAS
  VERSION 1.0.0
  ENTRY_FUNC imasPlugin
  DESCRIPTION ""
  EXAMPLE ""
  LIBNAME imas_plugin
  SOURCES
    imas_plugin.cpp
    imas_plugin.h
  EXTRA_INCLUDE_DIRS
  EXTRA_LINK_LIBS
)
