cmake_minimum_required(VERSION 3.0)

project(aoc-cli)

set(HEADERS
    HttpsRequest.hpp
    HtmlContent.hpp
    Printer.hpp
    HtmlFormatter.hpp
)

set(SOURCES
    HttpsRequest.cpp
    main.cpp
    HtmlContent.cpp
    Printer.cpp
    HtmlFormatter.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

find_package(CURL CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE PRIVATE CURL::libcurl cxxopts::cxxopts)
