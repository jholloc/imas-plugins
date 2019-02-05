find_package( UDA REQUIRED )

include( testing )

set( TESTS
  test_imas_tcvm
)

foreach( TEST ${TESTS} )
  BUILD_TEST( ${TEST} ${TEST}.cpp )
endforeach()
