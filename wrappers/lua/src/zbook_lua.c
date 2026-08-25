/**
 * @file zbook_lua.c
 * @brief Aggregates the enabled zbook Lua bindings into a single `zbook` table.
 *
 */

#include <lauxlib.h>
#include <zbook_lua.h>

#ifdef CONFIG_ZBOOK_LUA_SPI
#include <protocols/zbook_lua_spi.h>
#endif

#if defined(CONFIG_ZBOOK_LUA_SPI)
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

#if defined(CONFIG_ZBOOK_LUA_SPI)
	push_category_protocols(L);
	lua_setfield(L, -2, "protocols");
#endif

	return 1;
}