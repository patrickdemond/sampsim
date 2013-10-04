FILE( GLOB HEADER_FILENAMES "../*.h" )
FILE( GLOB IN_HEADER_FILENAMES "../*.h.in" )


# Include library paths
INCLUDE_DIRECTORIES(
  ${SAMPSIM_LIB_DIR}/UnitTest++/src
)

FOREACH( HEADER ${HEADER_FILENAMES} ${IN_HEADER_FILENAMES} )
  STRING( REGEX REPLACE "(.*/)|(\\.h)|(\\.in)" "" CLASS_NAME ${HEADER} )
  SET( TEST_NAME test_${CLASS_NAME} )
  IF( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cxx )
    MESSAGE( "Warning: did not find any unit testing for ${CLASS_NAME} class" )
  ELSE()
    ADD_EXECUTABLE( ${TEST_NAME} ${TEST_NAME}.cxx )
    TARGET_LINK_LIBRARIES(
      ${TEST_NAME} UnitTest++
      ${PROJECT_NAME}
    )
    ADD_TEST( ${CLASS_NAME} ${TEST_NAME} )
  ENDIF()
ENDFOREACH()
