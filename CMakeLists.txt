cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    aoc-cli-cpp
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    aoc-cli-cpp_aoc-cli-cpp
    source/aoc-cli-cpp.cpp
)
add_library(aoc-cli-cpp::aoc-cli-cpp ALIAS aoc-cli-cpp_aoc-cli-cpp)

include(GenerateExportHeader)
generate_export_header(
    aoc-cli-cpp_aoc-cli-cpp
    BASE_NAME aoc-cli-cpp
    EXPORT_FILE_NAME export/aoc-cli-cpp/aoc-cli-cpp_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(aoc-cli-cpp_aoc-cli-cpp PUBLIC AOC_CLI_CPP_STATIC_DEFINE)
endif()

set_target_properties(
    aoc-cli-cpp_aoc-cli-cpp PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME aoc-cli-cpp
    OUTPUT_NAME aoc-cli-cpp
)

target_include_directories(
    aoc-cli-cpp_aoc-cli-cpp ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    aoc-cli-cpp_aoc-cli-cpp SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(aoc-cli-cpp_aoc-cli-cpp PUBLIC cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(aoc-cli-cpp_aoc-cli-cpp PRIVATE fmt::fmt)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT aoc-cli-cpp_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of aoc-cli-cpp"
  )
endif()

include(cmake/dev-mode.cmake)
