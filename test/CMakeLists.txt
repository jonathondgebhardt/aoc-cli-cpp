cmake_minimum_required(VERSION 3.14)

project(aoc-cliTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(aoc-cli REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(aoc-cli_test source/aoc-cli_test.cpp)
target_link_libraries(
    aoc-cli_test PRIVATE
    aoc-cli::aoc-cli
    Catch2::Catch2WithMain
)
target_compile_features(aoc-cli_test PRIVATE cxx_std_23)

catch_discover_tests(aoc-cli_test)

# ---- End-of-file commands ----

add_folders(Test)
