cmake_minimum_required( VERSION 3.7 )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
  cmake_policy( SET CMP0074 OLD )
endif()

# Extra cmake modules to find libraries
set( CMAKE_MODULE_PATH
     ${CMAKE_MODULE_PATH}
     ${CMAKE_SOURCE_DIR}/cmake
     ${CMAKE_SOURCE_DIR}/cmake/Modules
)

########################################################################################################################
# Version and Machine information

include( GetGitRevisionDescription )

git_describe( GIT_TAG --tags )

if( "${GIT_TAG}" MATCHES "^.*NOTFOUND$" )
  message( "Failed to get git revision: ${GIT_TAG}" )
  set( GIT_TAG "0.0.0" )
endif()

# Strip off the SHA1 tag element if it exists
string( REGEX REPLACE "^([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" GIT_VERSION "${GIT_TAG}" )
if( NOT "${GIT_VERSION}" MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+" )
  message( WARNING "invalid git tag for version parsing: ${GIT_VERSION}" )
  set( GIT_VERSION "0.0.0" )
endif()

project( imas_plugins VERSION ${GIT_VERSION} )

########################################################################################################################
# CMake configuration
#
cmake_policy( SET CMP0042 NEW )
cmake_policy( SET CMP0048 NEW )
if( "${CMAKE_VERSION}" VERSION_GREATER "3.8.0" )
  cmake_policy( SET CMP0068 NEW )
endif()

if( CMAKE_C_COMPILER_ID STREQUAL "Intel" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" )
else()
  set( CMAKE_CXX_STANDARD 17 )
  set( CMAKE_CXX_STANDARD_REQUIRED ON )
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -pthread" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread" )

include( LibXml2StrPrintfConfig )
include( use_cxx11 )

use_cxx11()

########################################################################################################################
# Static Analysis

include( StaticAnalysis )

add_clang_format_target()

########################################################################################################################
# Plugins

include_directories( /usr/local/include )

add_subdirectory( source )

########################################################################################################################
# Create module

configure_file(
  ${CMAKE_SOURCE_DIR}/source/etc/module.in
  ${CMAKE_BINARY_DIR}/etc/uda-plugins/${PROJECT_VERSION}
  @ONLY
)

set( MODULE_PATH "modulefiles/uda-plugins" CACHE PATH "location to install module files to" )
set( MODULE_NAME "${PROJECT_VERSION}" CACHE STRING "name of the module file" )
string( CONFIGURE "${MODULE_PATH}" MODULE_PATH_EXPANDED @ONLY )

get_filename_component( MODULE_PATH_EXPANDED ${MODULE_PATH_EXPANDED} REALPATH BASE_DIR "${CMAKE_INSTALL_PREFIX}" )

install(
  FILES ${CMAKE_BINARY_DIR}/etc/uda-plugins/${PROJECT_VERSION}
  DESTINATION ${MODULE_PATH_EXPANDED}
  RENAME ${MODULE_NAME}
)
