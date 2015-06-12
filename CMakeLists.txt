cmake_minimum_required(VERSION 2.4)

if(NOT MSVC)
	add_definitions(-O3)
endif()

file(GLOB TINYGETTEXT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

add_library(tinygettext STATIC ${TINYGETTEXT_SOURCES})
