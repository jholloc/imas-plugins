# - Find UDA
#
# To provide the module with a hint about where to find your UDA
# installation, you can set the environment variables UDA_DIR. The
# Find module will then look in this path when searching for UDA paths
# and libraries.
#
# Find the UDA includes and libraries
#
#  UDA_INCLUDES        - Where to find memcached.h, etc
#  UDA_LIBRARY_DIRS    - Where to find the libraries
#  UDA_LIBRARIES       - Link these libraries when using UDA
#  UDA_FOUND           - True if UDA found
#
# Normal usage would be:
#  find_package (UDA REQUIRED)
#  target_link_libraries (name ${UDA_LIBRARIES})

# Check using pkg-config first
find_package( PkgConfig QUIET )
pkg_check_modules( UDA QUIET uda-fat-client )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( UDA DEFAULT_MSG UDA_LIBRARIES UDA_INCLUDE_DIRS )

mark_as_advanced( UDA_LIBRARIES UDA_INCLUDE_DIRS )
