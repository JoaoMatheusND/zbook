/*******************************************************************
 * @file lua_zbook.c
 *
 * @brief Aggregates the enabled zbook Lua bindings into a single `zbook` table.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <lauxlib.h>
#include <lua_zbook/lua_zbook.h>

#if defined(CONFIG_ZBOOK_LUA_UART)
#include <lua_zbook/protocols/lua_zbook_uart.h>

static void push_category_protocols(lua_State *L)
{
	lua_newtable(L);
#ifdef CONFIG_ZBOOK_LUA_UART
	luaopen_zbook_uart(L);
	lua_setfield(L, -2, "uart");
#endif
}
#endif

int luaopen_zbook(lua_State *L)
{
	lua_newtable(L);

#if defined(CONFIG_ZBOOK_LUA_UART)
	push_category_protocols(L);
	lua_setfield(L, -2, "protocols");
#endif

	return 1;
}