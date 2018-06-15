macro( BUILD_TEST NAME SOURCE )
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/setup.inc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/setup.inc"
    @ONLY
  )

  find_package( Boost REQUIRED )
  find_package( OpenSSL REQUIRED )
  find_package( UDA REQUIRED )

  include_directories( SYSTEM ${Boost_INCLUDE_DIR} )
  include_directories( ${UDA_CPP_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} )

  link_directories( ${UDA_CPP_LIBRARY_DIRS} )

  add_executable( ${NAME} ${HELPER_FILES} ${SOURCE} )
  target_link_libraries( ${NAME} PRIVATE
    ${UDA_CPP_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    test_helpers
  )
  target_compile_options( ${NAME} PRIVATE ${UDA_CPP_CFLAGS_OTHER}  )

  link_directories( ${UDA_FAT_CPP_LIBRARY_DIRS} )

  add_executable( fat_${NAME} ${HELPER_FILES} ${SOURCE} )
  target_link_libraries( fat_${NAME} PRIVATE
    ${UDA_FAT_CPP_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    test_helpers
  )
  target_compile_options( fat_${NAME} PRIVATE ${UDA_FAT_CPP_CFLAGS_OTHER}  )

  if( FAT_TESTS )
    add_test( ${NAME} fat_${NAME} )
  else()
    add_test( ${NAME} ${NAME} )
  endif()
endmacro( BUILD_TEST )