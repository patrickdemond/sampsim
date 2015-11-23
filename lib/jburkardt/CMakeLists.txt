cmake_minimum_required (VERSION 2.8)

project( jburkardt )
message( "Generating project ${PROJECT_NAME}" )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

list( APPEND SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/latin_center.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/latin_edge.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/latin_random.cpp
)

add_library( jburkardt STATIC ${SOURCES} )
