FILE( GLOB HEADER_FILENAMES "../*.h" )

# Include library paths
INCLUDE_DIRECTORIES(
  ${SAMPSIM_LIB_DIR}/UnitTest++/src
)

FOREACH( HEADER ${HEADER_FILENAMES} )
  STRING( REGEX REPLACE "(.*/)|(\\.h)" "" CLASS_NAME ${HEADER} )
  SET( TEST_NAME test_${CLASS_NAME} )
  IF( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cxx )
    MESSAGE( "Warning: did not find any unit testing for sample::${CLASS_NAME} class" )
  ELSE()
    ADD_EXECUTABLE( ${TEST_NAME} ${TEST_NAME}.cxx )
    TARGET_LINK_LIBRARIES(
      ${TEST_NAME} UnitTest++
      ${PROJECT_NAME}
      ${PROJECT_NAME}_sample
    )
    ADD_TEST( sample::${CLASS_NAME} ${TEST_NAME} )
  ENDIF()
ENDFOREACH()
