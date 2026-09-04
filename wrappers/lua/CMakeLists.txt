target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/zbook_lua.c)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Gathers ZBOOK_LUA_PROTOCOL_DEFINE() entries into one contiguous,
# STRUCT_SECTION_FOREACH()-iterable region (see include/zbook_lua_protocol.h).
zephyr_linker_sources(DATA_SECTIONS ${CMAKE_CURRENT_SOURCE_DIR}/zbook_lua_protocol.ld)

target_sources_ifdef(CONFIG_LUA_ZBOOK_UART app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/lua_zbook_uart.c
)

target_sources_ifdef(CONFIG_LUA_ZBOOK_SPI app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/lua_zbook_spi.c
)
