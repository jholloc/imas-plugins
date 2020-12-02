cmake_minimum_required( VERSION 3.0 )

if( ${CMAKE_VERSION} VERSION_GREATER 3.11 )
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

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -pthread" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread" )

include( LibXml2StrPrintfConfig )
include( use_cxx11 )

use_cxx11()

########################################################################################################################

include_directories( /usr/local/include )

add_subdirectory( source )
