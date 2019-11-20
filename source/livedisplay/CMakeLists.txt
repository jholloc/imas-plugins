include( plugins )

add_definitions( -Wno-error )

uda_plugin(
  NAME LIVEDISPLAY
  ENTRY_FUNC livedisplay
  DESCRIPTION "Live Display IMAS MAGNETICS IDS plugin"
  EXAMPLE "LIVEDISPLAY::get()"
  LIBNAME livedisplay_plugin
  SOURCES livedisplay.c
  EXTRA_INCLUDE_DIRS
  EXTRA_LINK_LIBS
    uda_client
)
