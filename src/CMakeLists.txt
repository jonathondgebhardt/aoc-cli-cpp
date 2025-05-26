cmake_minimum_required(VERSION 3.0...3.5)

project(aoc-cli)

add_subdirectory(client)

if(ENABLE_BUILD_TESTS)
    add_subdirectory(test)
endif()

add_executable(${PROJECT_NAME} main.cpp)

find_package(cxxopts CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts::cxxopts aoc-client)
