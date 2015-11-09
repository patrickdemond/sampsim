CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( sampsim )

# Set the version
SET( SAMPSIM_VERSION_MAJOR 0 )
SET( SAMPSIM_VERSION_MINOR 3 )
SET( SAMPSIM_VERSION_PATCH 0 )

# Look in the build directory for cmake modules
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/" )

SET( SAMPSIM_ROOT_DIR     ${PROJECT_SOURCE_DIR}/.. )
SET( SAMPSIM_AUX_DIR      ${SAMPSIM_ROOT_DIR}/aux )
SET( SAMPSIM_SRC_DIR      ${SAMPSIM_ROOT_DIR}/src )
SET( SAMPSIM_LIB_DIR      ${SAMPSIM_ROOT_DIR}/lib )
SET( SAMPSIM_DOC_DIR      ${SAMPSIM_ROOT_DIR}/doc )
SET( SAMPSIM_DOXY_DIR     ${PROJECT_BINARY_DIR}/doxygen )
SET( SAMPSIM_EXAMPLES_DIR ${SAMPSIM_ROOT_DIR}/examples )
SET( SAMPSIM_CONFIG_DIR   ${PROJECT_BINARY_DIR}/config )
SET( SAMPSIM_TESTING_DIR  ${PROJECT_BINARY_DIR}/Testing )

# See if gnuplot is installed
FIND_PACKAGE( Gnuplot QUIET )

# Build libs
SUBDIRS( ${SAMPSIM_LIB_DIR} )

# Copy all config files to build
FILE( GLOB CONFIG_FILENAMES "${SAMPSIM_AUX_DIR}/*.conf" )
FOREACH( CONFIG ${CONFIG_FILENAMES} )
  STRING( REPLACE
    ${SAMPSIM_AUX_DIR}
    ${SAMPSIM_CONFIG_DIR}
    DESTINATION
    ${CONFIG} )
  CONFIGURE_FILE( ${CONFIG} ${DESTINATION} COPYONLY )
ENDFOREACH()

# Copy the hypercube.sh and build.sh scripts to the config directory
FILE( COPY ${SAMPSIM_AUX_DIR}/hypercube.sh
      DESTINATION ${SAMPSIM_CONFIG_DIR}
      FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_EXECUTE
                       WORLD_READ WORLD_EXECUTE )
FILE( COPY ${SAMPSIM_AUX_DIR}/build.sh
      DESTINATION ${SAMPSIM_CONFIG_DIR} )

SET( SAMPSIM_EXECUTABLES
  generate
  arc_epi_sample
  circle_gps_sample
  random_sample
  square_gps_sample
  strip_epi_sample
)

# Include library paths
INCLUDE_DIRECTORIES(
  ${SAMPSIM_SRC_DIR}
  ${SAMPSIM_LIB_DIR}/local
  ${PROJECT_BINARY_DIR}/lib/local
  ${PROJECT_BINARY_DIR}/lib/local/sample
)

# We're using <random>, the auto keyword, constant iterators, etc, so we need c++11
SET( CMAKE_CXX_FLAGS "-std=c++0x " )

# Targets
FOREACH( EXECUTABLE ${SAMPSIM_EXECUTABLES} )
  ADD_EXECUTABLE( ${EXECUTABLE} ${SAMPSIM_SRC_DIR}/${EXECUTABLE}.cxx )
  TARGET_LINK_LIBRARIES( ${EXECUTABLE}
    ${PROJECT_NAME}
    ${PROJECT_NAME}_sample
  )
ENDFOREACH( EXECUTABLE IN ${SAMPSIM_EXECUTABLES} )

INSTALL( TARGETS ${SAMPSIM_EXECUTABLES} RUNTIME DESTINATION bin )

ADD_CUSTOM_TARGET( dist
  COMMAND git archive --prefix=${SAMPSIM_ARCHIVE_NAME}/ HEAD
    | bzip2 > ${CMAKE_BINARY_DIR}/${SAMPSIM_ARCHIVE_NAME}.tar.bz2
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Build doxygen documentation ?
INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)
IF( BUILD_DOCUMENTATION )

  SET( HAVE_DOT_YESNO NO )
  IF( DOT )
    SET( HAVE_DOT_YESNO YES )
    IF( NOT DOT_PATH )
      GET_FILENAME_COMPONENT( DOT_PATH ${DOT} PATH )
    ENDIF( NOT DOT_PATH )
  ENDIF( DOT )

  CONFIGURE_FILE(
    ${SAMPSIM_DOC_DIR}/doc_mainpage.dox.in
    ${SAMPSIM_DOXY_DIR}/doc_mainpage.dox )

  CONFIGURE_FILE(
    ${SAMPSIM_DOC_DIR}/doc_makeall.cmake.in
    ${SAMPSIM_DOXY_DIR}/doc_makeall.cmake
    @ONLY )
  
  CONFIGURE_FILE(
    ${SAMPSIM_DOC_DIR}/doc_mainpage.dox.in
    ${SAMPSIM_DOXY_DIR}/doc_mainpage.dox )

  SET( DOXY_INPUT_SOURCE 
    ${SAMPSIM_LIB_DIR}/local
    ${SAMPSIM_LIB_DIR}/local/sample
    ${SAMPSIM_DOXY_DIR}/doc_mainpage.dox )

  string( REPLACE ";" "\" \"" SAMPSIM_DOXY_LIST "${DOXY_INPUT_SOURCE}" )
  
  CONFIGURE_FILE(
    ${SAMPSIM_DOC_DIR}/config.dox.in
    ${SAMPSIM_DOXY_DIR}/config.dox )

# Uncommenting this block will force a build of the documentation
# every time cmake is run in the build directory
# 
#  execute_process(COMMAND 
#    ${CMAKE_COMMAND} -P ${SAMPSIM_DOXY_DIR}/doc_makeall.cmake
#    DEPENDS ${SAMPSIM_DOXY_DIR}/doc_makeall.cmake 
#    WORKING DIR ${SAMPSIM_DOXY_DIR}/doxygen 
#    RESULT_VARIABLE rv )

ENDIF( BUILD_DOCUMENTATION )

OPTION( BUILD_TESTING "Build the testing tree." OFF )
MARK_AS_ADVANCED( BUILD_TESTING )
IF( BUILD_TESTING )
  ENABLE_TESTING()

  # copy the multisampling tree from source to build
  FILE( COPY ${SAMPSIM_EXAMPLES_DIR}/multisampling DESTINATION ${SAMPSIM_TESTING_DIR} )

  # copy the visualization tree from source to build (but only if gnuplot is available)
  IF( GNUPLOT_FOUND )
    FILE( COPY ${SAMPSIM_EXAMPLES_DIR}/visualization DESTINATION ${SAMPSIM_TESTING_DIR} )
  ENDIF( GNUPLOT_FOUND )
ENDIF( BUILD_TESTING )
