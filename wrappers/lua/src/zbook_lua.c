/*******************************************************************
 * @file zbook_lua.c
 *
 * @brief Implements the Lua Wrapper Interface for the zbook library.
 * @author José Félix de Oliveira Neto (josefelix.neto@edge.ufal.br)
 * @version 0.1
 * @date 25/08/26
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <lauxlib.h>
#include <zbook_lua.h>
#include <zbook_lua_protocol.h>

static void push_category_protocols(lua_State *L)
{
	lua_newtable(L);

	STRUCT_SECTION_FOREACH(zbook_lua_protocol, proto)
	{
		proto->open(L);
		lua_setfield(L, -2, proto->name);
	}
}

int luaopen_zbook(lua_State *L)
{
	lua_newtable(L);

	size_t protocol_count;

	STRUCT_SECTION_COUNT(zbook_lua_protocol, &protocol_count);

	if (protocol_count > 0) {
		push_category_protocols(L);
		lua_setfield(L, -2, "protocols");
	}

	return 1;
}