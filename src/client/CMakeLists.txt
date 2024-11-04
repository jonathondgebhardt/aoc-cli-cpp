cmake_minimum_required(VERSION 3.0...3.5)

project(aoc-client)

set(HEADERS
    Client.hpp
    HtmlContent.hpp
    HtmlFormatter.hpp
    HttpsRequest.hpp
    Printer.hpp
    RequestManager.hpp
    System.hpp
)

set(SOURCES
    Client.cpp
    HtmlContent.cpp
    HtmlFormatter.cpp
    HttpsRequest.cpp
    Printer.cpp
    RequestManager.cpp
)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

find_package(CURL CONFIG REQUIRED)
find_package(LibXml2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl LibXml2::LibXml2)