# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(ZBOOK_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)
set(BOARD_ROOT ${ZBOOK_ROOT})

if(NOT DEFINED BOARD)
	set(BOARD zbook/rp2350b/m33)
endif()

include($ENV{ZEPHYR_BASE}/modules/lua_zephyr/luaz.cmake)
luaz_define_source_thread(src/uart_sample.lua)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(uart_sample)

target_include_directories(app PRIVATE
	${ZBOOK_ROOT}/interface/includes
	${ZBOOK_ROOT}/wrappers/lua/include
)

# This sample builds the zbook lua wrapper directly, outside of zbook's own
# Kconfig tree, so CONFIG_ZBOOK_LUA_UART is never defined by menuconfig.
target_compile_definitions(app PRIVATE CONFIG_ZBOOK_LUA_UART=1)

target_sources(app PRIVATE
	${ZBOOK_ROOT}/interface/src/protocols/zbook_uart.c
	${ZBOOK_ROOT}/wrappers/lua/src/lua_zbook.c
	${ZBOOK_ROOT}/wrappers/lua/src/protocols/lua_zbook_uart.c
	src/setup.c
)

luaz_generate_threads()
