file(GLOB_RECURSE ZBOOK_LUA_WRAPPER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

target_sources(app PRIVATE ${ZBOOK_LUA_WRAPPER_SOURCES})
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_zbook.c)

target_sources_ifdef(CONFIG_ZBOOK_LUA_UART app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/zbook_lua_uart.c
)
