find_package( UDA REQUIRED )
find_package( LibSSH QUIET )

if( NOT LIBSSH_FOUND )
  message( WARNING "Libssh not found - skipping west_tunnel plugin" )
  return()
endif()

include( plugins )

set( SOURCES
  west_tunnel.c west_tunnel_ssh.c west_tunnel_ssh_server.c
)

uda_plugin(
  NAME WEST_TUNNEL
  ENTRY_FUNC west_tunnel
  DESCRIPTION ""
  EXAMPLE "WEST_TUNNEL::read()"
  LIBNAME west_tunnel_plugin
  SOURCES ${SOURCES}
  VERSION ${PROJECT_VERSION}
  EXTRA_INCLUDE_DIRS
    ${UDA_CLIENT_INCLUDE_DIRS}
    ${LIBSSH_INCLUDE_DIRS}
  EXTRA_LINK_LIBS
    ${UDA_CLIENT_LIBRARIES}
    ${LIBSSH_LIBRARIES}
    resolv
)

