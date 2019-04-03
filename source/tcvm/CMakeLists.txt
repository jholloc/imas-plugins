find_package( UDA REQUIRED )

include( plugins )

set( SOURCES
  tcvm_matlab_engine.c
  tcvm_plugin.c
)

set( MATLAB
  $ENV{MATLAB}
)

include_directories( SYSTEM )

add_compile_options( -fexceptions -fPIC -fno-omit-frame-pointer -pthread -g )

uda_plugin(
  NAME TCVM
  ENTRY_FUNC tcvmPlugin
  DESCRIPTION "Use MATLAB to assemble the IDS and store it in memory"
  EXAMPLE "TCVM::read()"
  LIBNAME tcvm_plugin
  SOURCES ${SOURCES}
  CONFIG_FILE tcvm.cfg
  VERSION 1.0.0
  EXTRA_DEFINITIONS
    -DUSE_MEX_CMD
    -D_GNU_SOURCE
    -D_MATLAB_MEX_FILE
    -DNDEBUG
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${MATLAB}/extern/include
  EXTRA_LINK_DIRS
    ${MATLAB}/extern/bin/glnxa64
    ${MATLAB}/bin/glnxa64
  EXTRA_LINK_LIBS
    -pthread
    -Wl,--no-undefined
    -g
    ${UDA_CLIENT_LIBRARIES}
    pthread
    -Wl,--as-needed 
    -Wl,-rpath-link,${MATLAB}/extern/bin/glnxa64
    mx
    eng
    -Wl,-rpath-link,${MATLAB}/bin/glnxa64
)

add_subdirectory( tests )
