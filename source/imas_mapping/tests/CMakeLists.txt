find_package( Boost REQUIRED )
find_package( UDA REQUIRED )

include( testing )

include_directories( SYSTEM ${Boost_INCLUDE_DIR} )
include_directories( ${UDA_CPP_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

link_directories( ${UDA_CPP_LIBRARY_DIRS} )

add_executable( test_imas_mapping test_imas_mapping.cpp )
target_link_libraries( test_imas_mapping PRIVATE
  ${UDA_CPP_LIBRARIES}
  ${UDA_PLUGINS_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  test_helpers
  imas_mapping_plugin
)
target_compile_options( test_imas_mapping PRIVATE ${UDA_PLUGINS_CFLAGS_OTHER}  )

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/setup.inc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/setup.inc"
  @ONLY
)

add_test( test_imas_mapping test_imas_mapping )
