SET( SAMPSIM_LIB_NAME "${PROJECT_NAME}" )

# Find required packages
FIND_PACKAGE( JSONCpp REQUIRED )

IF( GNUPLOT_FOUND )
  SET( GNUPLOT_AVAILABLE "true" )
ELSE( GNUPLOT_FOUND )
  SET( GNUPLOT_AVAILABLE "false" )
ENDIF( GNUPLOT_FOUND )

# Configure the utitlities header
CONFIGURE_FILE( utilities.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/utilities.h @ONLY IMMEDIATE )

SET( SAMPSIM_LIB_SOURCE
  base_object.cxx
  building.cxx
  building_tree.cxx
  coordinate.cxx
  distribution.cxx
  household.cxx
  individual.cxx
  line.cxx
  options.cxx
  population.cxx
  summary.cxx
  tile.cxx
  town.cxx
  trend.cxx
  utilities.cxx
)

# Define abstract classes in the following block
SET_SOURCE_FILES_PROPERTIES(
  base_object.cxx
  model_object.cxx
  ABSTRACT
)

# We're using <random>, the auto keyword, constant iterators, etc, so we need c++11
SET( CMAKE_CXX_FLAGS "-std=c++0x " )

# Include library paths
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${JSONCPP_INCLUDE_DIR}
)

ADD_LIBRARY( ${SAMPSIM_LIB_NAME} SHARED ${SAMPSIM_LIB_SOURCE} )
TARGET_LINK_LIBRARIES( ${SAMPSIM_LIB_NAME}
  ${JSONCPP_LIBRARIES}
)
INSTALL( TARGETS ${SAMPSIM_LIB_NAME} DESTINATION lib )

ADD_SUBDIRECTORY( sample )
IF( BUILD_TESTING )
  ADD_SUBDIRECTORY( testing )
ENDIF()
