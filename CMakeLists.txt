cmake_minimum_required(VERSION 3.20.0)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED BOARD)
    set(BOARD zbook/rp2350b/m33)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zbook)

file(GLOB_RECURSE ZBOOK_INTERFACE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/interface/src/*.c)

target_sources(app PRIVATE ${SOURCE_FILES})

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/interface/includes)

add_subdirectory_ifdef(CONFIG_ZBOOK_LUA_UART wrappers/lua)
