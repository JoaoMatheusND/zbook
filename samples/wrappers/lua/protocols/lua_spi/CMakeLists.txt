cmake_minimum_required(VERSION 3.20.0)

get_filename_component(ZBOOK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE)

set(BOARD_ROOT ${ZBOOK_ROOT})

if(NOT DEFINED BOARD)
    set(BOARD zbook/rp2350b/m33)
endif()

include($ENV{ZEPHYR_BASE}/modules/lua_zephyr/luaz.cmake)
luaz_define_source_thread(src/lua_spi.lua)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zbook_lua_spi_sample)

file(GLOB_RECURSE ZBOOK_INTERFACE_SOURCES ${ZBOOK_ROOT}/interface/src/*.c)
target_sources(app PRIVATE ${ZBOOK_INTERFACE_SOURCES})
target_include_directories(app PRIVATE ${ZBOOK_ROOT}/interface/includes)

file(GLOB_RECURSE ZBOOK_LUA_WRAPPER_SOURCES ${ZBOOK_ROOT}/wrappers/lua/src/*.c)
target_sources(app PRIVATE ${ZBOOK_LUA_WRAPPER_SOURCES})
target_include_directories(app PRIVATE ${ZBOOK_ROOT}/wrappers/lua/include)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/zbook_lua_setup.c)

luaz_generate_threads()
