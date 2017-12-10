SET( SAMPSIM_SAMPLE_LIB_NAME "${PROJECT_NAME}_sample" )

SET( SAMPSIM_SAMPLE_LIB_SOURCE
  arc_epi.cxx
  circle_gps.cxx
  direction_epi.cxx
  epi.cxx
  gps.cxx
  random.cxx
  sized_sample.cxx
  square_gps.cxx
  strip_epi.cxx
  sample.cxx
)

# Define abstract classes in the following block
SET_SOURCE_FILES_PROPERTIES(
  direction_epi.cxx
  epi.cxx
  gps.cxx
  sized_sample.cxx
  sample.cxx
  ABSTRACT
)

# Include library paths
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${JSONCPP_INCLUDE_DIR}
)

ADD_LIBRARY( ${SAMPSIM_SAMPLE_LIB_NAME} SHARED ${SAMPSIM_SAMPLE_LIB_SOURCE} )
TARGET_LINK_LIBRARIES( ${SAMPSIM_SAMPLE_LIB_NAME}
  ${SAMPSIM_LIB_NAME}
  ${JSONCPP_LIBRARIES}
  ${LibArchive_LIBRARIES}
)
INSTALL( TARGETS ${SAMPSIM_SAMPLE_LIB_NAME} DESTINATION lib )

IF( BUILD_TESTING )
  ADD_SUBDIRECTORY( testing )
ENDIF()
