cmake_minimum_required(VERSION 3.0...3.5)

project(aoc-client)

set(HEADERS
    HttpsRequest.hpp
    HtmlContent.hpp
    Printer.hpp
    HtmlFormatter.hpp
    RequestManager.hpp
    System.hpp
    Client.hpp
)

set(SOURCES
    HttpsRequest.cpp
    HtmlContent.cpp
    Printer.cpp
    HtmlFormatter.cpp
    RequestManager.cpp
    Client.cpp
)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

find_package(CURL CONFIG REQUIRED)
find_package(LibXml2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl LibXml2::LibXml2)