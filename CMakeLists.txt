cmake_minimum_required(VERSION 3.20.0)

if(NOT DEFINED BOARD)
    set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/boards)
    set(BOARD zbook/rp2350b/m33)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zbook)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.C)

target_sources(app PRIVATE ${SOURCE_FILES})

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)