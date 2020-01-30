find_package( Boost QUIET COMPONENTS filesystem system )

add_definitions( -DSERVERBUILD )

include_directories( .. )
link_directories( .. ${UDA_LIBRARY_DIRS} )

add_executable( imas_partial_tests test_plugin.cpp )
target_link_libraries( imas_partial_tests imas_partial_plugin ${UDA_LIBRARIES} ${UDA_PLUGINS_LIBRARIES} ${Boost_LIBRARIES} )

BUILD_TEST( tests test.cpp )
