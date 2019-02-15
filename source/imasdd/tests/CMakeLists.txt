set( SOURCES tests.cpp )

add_definitions( -DSERVERBUILD )

include_directories( .. )
link_directories( .. ${UDA_LIBRARY_DIRS} )

add_executable( imasdd_test ${SOURCES} )
target_link_libraries( imasdd_test imasdd_plugin ${UDA_LIBRARIES} ${UDA_PLUGINS_LIBRARIES} )
