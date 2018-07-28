cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(PlayingCards)

set(CMAKE_CXX_STANDARD 11)

file(GLOB includes
	"${CMAKE_SOURCE_DIR}/includes/PlayingCards/*.hpp"
	"${CMAKE_SOURCE_DIR}/includes/PlayingCards/*.h")

file(GLOB srcs "./" "*.hpp" "*.h" "*.cpp")
add_library(${PROJECT_NAME} STATIC ${includes} ${srcs})

target_include_directories(
	${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/includes/PlayingCards/")

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin"
	EXECUTABLE_OUTPUT_PATH		"${CMAKE_SOURCE_DIR}/bin"
)