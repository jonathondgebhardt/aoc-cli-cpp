cmake_minimum_required(VERSION 3.0...3.5)

# Must be done before the first call to project()
# https://github.com/Microsoft/vcpkg#vcpkg-with-visual-studio-cmake-projects
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(aoc-cli-cpp CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)

if(NOT WIN32)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO: Add Windows support
if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
	set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall -Werror")
	set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
endif()

set(ENABLE_BUILD_TESTS ON CACHE BOOL "Whether tests should be built")

add_subdirectory(src)
