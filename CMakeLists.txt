cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    aoc-cli
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    aoc-cli_aoc-cli
    #source/main.cpp
    source/Client.cpp
    source/HtmlContent.cpp
    source/HtmlFormatter.cpp
    source/HttpsRequest.cpp
    source/Printer.cpp
    source/RequestManager.cpp
)
add_library(aoc-cli::aoc-cli ALIAS aoc-cli_aoc-cli)

include(GenerateExportHeader)
generate_export_header(
    aoc-cli_aoc-cli
    BASE_NAME aoc-cli
    EXPORT_FILE_NAME export/aoc-cli/aoc-cli_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(aoc-cli_aoc-cli PUBLIC AOC_CLI_CPP_STATIC_DEFINE)
endif()

set_target_properties(
    aoc-cli_aoc-cli PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME aoc-cli
    OUTPUT_NAME aoc-cli
)

target_include_directories(
    aoc-cli_aoc-cli ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    aoc-cli_aoc-cli SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(aoc-cli_aoc-cli PUBLIC cxx_std_23)

find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)
target_link_libraries(aoc-cli_aoc-cli PRIVATE CURL::libcurl LibXml2::LibXml2)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT aoc-cli_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of aoc-cli"
  )
endif()

include(cmake/dev-mode.cmake)
