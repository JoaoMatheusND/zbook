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

#ifdef CONFIG_LUA_ZBOOK_SPI
#include <protocols/lua_zbook_spi.h>
#endif

#if defined(CONFIG_LUA_ZBOOK_SPI)
static void push_category_protocols(lua_State *L)
{
	lua_newtable(L);

	luaopen_zbook_spi(L);
	lua_setfield(L, -2, "spi");
}
#endif

int luaopen_zbook(lua_State *L)
{
	lua_newtable(L);

#if defined(CONFIG_LUA_ZBOOK_SPI)
	push_category_protocols(L);
	lua_setfield(L, -2, "protocols");
#endif

	return 1;
}