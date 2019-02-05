find_package( UDA REQUIRED )

include( plugins )

set( SOURCES
  tcvm_plugin.c
)

set( MATLAB
  /gw/swvend/matlab/r2017b
)

include_directories( SYSTEM )

uda_plugin(
  NAME TCVM
  ENTRY_FUNC tcvmPlugin
  DESCRIPTION "Use MATLAB to assemble the IDS and store it in memory"
  EXAMPLE "TCVM::read()"
  LIBNAME tcvm_plugin
  SOURCES ${SOURCES}
  VERSION 1.0.0
  EXTRA_DEFINITIONS
    -DUSE_MEX_CMD
    -D_GNU_SOURCE
    -D_MATLAB_MEX_FILE
    -DNDEBUG
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${MATLAB}/extern/include
    /pfs/work/g2amerle/matlabengine/c
  EXTRA_LINK_DIRS
    ${MATLAB}/extern/bin/glnxa64
    ${MATLAB}/bin/glnxa64
    /pfs/work/g2amerle/matlabengine/c
  EXTRA_LINK_LIBS
    -pthread
    -Wl,--no-undefined
    -g
    -Wl,-Bdynamic
    testStart
    ${UDA_CLIENT_LIBRARIES}
    pthread
    -Wl,--as-needed 
    -Wl,-rpath-link,${MATLAB}/extern/bin/glnxa64
    eng
    -Wl,-rpath-link,${MATLAB}/bin/glnxa64
)

add_compile_options( -fexceptions -fPIC -fno-omit-frame-pointer -pthread -g )

add_subdirectory( tests )
