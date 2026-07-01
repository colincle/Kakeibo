cmake_minimum_required(VERSION 3.16)
project(Kakeibo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Concurrent)

file(GLOB_RECURSE SOURCES
	src/*.cpp
	include/**/*.hpp
)

add_executable(Kakeibo ${SOURCES})

foreach(_file IN LISTS SOURCES)
	if(_file MATCHES "^${CMAKE_SOURCE_DIR}/src/.*\\.cpp$" OR
	   _file MATCHES "^${CMAKE_SOURCE_DIR}/include/.*\\.hpp$")
		set_source_files_properties(${_file} PROPERTIES COMPILE_FLAGS
		"-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wnull-dereference -Wold-style-cast -Wdouble-promotion -Wformat=2 -Wcast-align -Wmissing-declarations"
		)
	endif()
endforeach()

target_include_directories(Kakeibo PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/core
	${CMAKE_CURRENT_SOURCE_DIR}/include/ui
	${CMAKE_CURRENT_SOURCE_DIR}/include/uiElements
	${CMAKE_CURRENT_SOURCE_DIR}/external/json
)

target_link_libraries(Kakeibo
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::Network
)

# --- Tests -------------------------------------------------------------------
# The parser is pure logic with no Qt dependency, so it can be unit tested in
# isolation. Run with: cmake --build build && ctest --test-dir build
enable_testing()

add_executable(parser_tests
	tests/test_parser.cpp
	src/core/Parser.cpp
)

target_include_directories(parser_tests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/core
	${CMAKE_CURRENT_SOURCE_DIR}/external/json
)

add_test(NAME parser_tests COMMAND parser_tests)
