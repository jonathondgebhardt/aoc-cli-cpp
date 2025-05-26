cmake_minimum_required(VERSION 3.14)

project(aoc-cli-cppTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(aoc-cli-cpp REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(aoc-cli-cpp_test source/aoc-cli-cpp_test.cpp)
target_link_libraries(
    aoc-cli-cpp_test PRIVATE
    aoc-cli-cpp::aoc-cli-cpp
    Catch2::Catch2WithMain
)
target_compile_features(aoc-cli-cpp_test PRIVATE cxx_std_20)

catch_discover_tests(aoc-cli-cpp_test)

# ---- End-of-file commands ----

add_folders(Test)
