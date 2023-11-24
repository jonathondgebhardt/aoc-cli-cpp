cmake_minimum_required(VERSION 3.0...3.5)

project(aoc-cli)

set(HEADERS
    HttpsRequest.hpp
    HtmlContent.hpp
    Printer.hpp
    HtmlFormatter.hpp
    AocHttpsRequest.hpp
    Throttler.hpp
)

set(SOURCES
    HttpsRequest.cpp
    main.cpp
    HtmlContent.cpp
    Printer.cpp
    HtmlFormatter.cpp
    AocHttpsRequest.cpp
    Throttler.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

find_package(CURL CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(LibXml2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE PRIVATE CURL::libcurl cxxopts::cxxopts LibXml2::LibXml2)
