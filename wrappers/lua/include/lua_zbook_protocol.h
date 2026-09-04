/*******************************************************************
 * @file lua_zbook_protocol.h
 *
 * @brief Self-registration mechanism for zbook.protocols.* Lua bindings.
 *
 * Each protocol binding (lua_zbook_spi.c, lua_zbook_uart.c, ...) declares
 * itself with ZBOOK_LUA_PROTOCOL_DEFINE(); zbook_lua.c iterates the
 * resulting Zephyr iterable section to build zbook.protocols without any
 * per-protocol #ifdef. Adding a new protocol only touches its own .c file.
 *
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 04/09/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/
#ifndef LUA_ZBOOK_PROTOCOL_H
#define LUA_ZBOOK_PROTOCOL_H

#include <lua.h>
#include <zephyr/sys/iterable_sections.h>

/** @brief One entry in the zbook.protocols.* registry. */
struct zbook_lua_protocol {
	const char *name;   /**< Field name under zbook.protocols (e.g. "spi"). */
	lua_CFunction open; /**< luaopen_zbook_<proto> function. */
};

/**
 * @brief Register a protocol under zbook.protocols.<_name>.
 *
 * @param _name Unquoted field name (e.g. spi, uart).
 * @param _open luaopen_zbook_<proto> function for this protocol.
 */
#define ZBOOK_LUA_PROTOCOL_DEFINE(_name, _open)                                                  \
	static STRUCT_SECTION_ITERABLE(zbook_lua_protocol, _zbook_lua_protocol_##_name) = {       \
		.name = #_name,                                                                   \
		.open = (_open),                                                                  \
	}

#endif /* LUA_ZBOOK_PROTOCOL_H */
