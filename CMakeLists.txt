cmake_minimum_required(VERSION 2.4)

add_definitions(-O3)

file(GLOB TINYGETTEXT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

add_library(tinygettext STATIC ${TINYGETTEXT_SOURCES})
